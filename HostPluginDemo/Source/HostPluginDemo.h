#pragma once

void scanForPlugins(juce::KnownPluginList& pluginList);  // Function declaration



//==============================================================================
enum class EditorStyle { thisWindow, newWindow };

class HostAudioProcessorImpl : public AudioProcessor,
                               private ChangeListener
{
public:
    HostAudioProcessorImpl()
        : AudioProcessor (BusesProperties().withInput  ("Input",  AudioChannelSet::stereo(), true)
                                           .withOutput ("Output", AudioChannelSet::stereo(), true))
    {
        appProperties.setStorageParameters ([&]
        {
            PropertiesFile::Options opt;
            opt.applicationName = getName();
            opt.commonToAllUsers = true;
            opt.doNotSave = true;
            opt.filenameSuffix = ".props";
            opt.ignoreCaseOfKeyNames = false;
            opt.storageFormat = PropertiesFile::StorageFormat::storeAsXML;
            opt.osxLibrarySubFolder = "Application Support";
            return opt;
        }());

        pluginFormatManager.addDefaultFormats();
        

        pluginFormatManager.addFormat(new juce::AudioUnitPluginFormat());
        


        if (auto savedPluginList = appProperties.getUserSettings()->getXmlValue ("pluginList-new"))
            pluginList.recreateFromXml (*savedPluginList);

        MessageManagerLock lock;
        pluginList.addChangeListener (this);
        scanForPlugins(pluginList);
    }




    void prepareToPlay (double sr, int bs) final
    {
        const ScopedLock sl (innerMutex);

        active = true;

        if (inner != nullptr)
        {
            inner->setRateAndBufferSizeDetails (sr, bs);
            inner->prepareToPlay (sr, bs);
        }
    }

    void releaseResources() final
    {
        const ScopedLock sl (innerMutex);

        active = false;

        if (inner != nullptr)
            inner->releaseResources();
    }

    void reset() final
    {
        const ScopedLock sl (innerMutex);

        if (inner != nullptr)
            inner->reset();
    }

    // In this example, we don't actually pass any audio through the inner processor.
    // In a 'real' plugin, we'd need to add some synchronisation to ensure that the inner
    // plugin instance was never modified (deleted, replaced etc.) during a call to processBlock.
    void processBlock (AudioBuffer<float>&, MidiBuffer&) final
    {
        jassert (! isUsingDoublePrecision());
    }

    void processBlock (AudioBuffer<double>&, MidiBuffer&) final
    {
        jassert (isUsingDoublePrecision());
    }

    bool hasEditor() const override                                   { return false; }
    AudioProcessorEditor* createEditor() override                     { return nullptr; }

    const String getName() const final                                { return "HostPluginDemo"; }
    bool acceptsMidi() const final                                    { return true; }
    bool producesMidi() const final                                   { return true; }
    double getTailLengthSeconds() const final                         { return 0.0; }

    int getNumPrograms() final                                        { return 0; }
    int getCurrentProgram() final                                     { return 0; }
    void setCurrentProgram (int) final                                {}
    const String getProgramName (int) final                           { return "None"; }
    void changeProgramName (int, const String&) final                 {}

    void getStateInformation (MemoryBlock& destData) final
    {
        const ScopedLock sl (innerMutex);

        XmlElement xml ("state");

        if (inner != nullptr)
        {
            xml.setAttribute (editorStyleTag, (int) editorStyle);
            xml.addChildElement (inner->getPluginDescription().createXml().release());
            xml.addChildElement ([this]
            {
                MemoryBlock innerState;
                inner->getStateInformation (innerState);

                auto stateNode = std::make_unique<XmlElement> (innerStateTag);
                stateNode->addTextElement (innerState.toBase64Encoding());
                return stateNode.release();
            }());
        }

        const auto text = xml.toString();
        destData.replaceAll (text.toRawUTF8(), text.getNumBytesAsUTF8());
    }

    void setStateInformation (const void* data, int sizeInBytes) final
    {
        const ScopedLock sl (innerMutex);

        auto xml = XmlDocument::parse (String (CharPointer_UTF8 (static_cast<const char*> (data)), (size_t) sizeInBytes));

        if (auto* pluginNode = xml->getChildByName ("PLUGIN"))
        {
            PluginDescription pd;
            pd.loadFromXml (*pluginNode);

            MemoryBlock innerState;
            innerState.fromBase64Encoding (xml->getChildElementAllSubText (innerStateTag, {}));

            setNewPlugin (pd,
                          (EditorStyle) xml->getIntAttribute (editorStyleTag, 0),
                          innerState);
        }
    }

    void setNewPlugin(const PluginDescription& pd, EditorStyle where, const MemoryBlock& mb = {})
    {
        const ScopedLock sl(innerMutex);
    
        const auto callback = [this, where, mb](std::unique_ptr<AudioPluginInstance> instance, const String& error)
        {
            if (error.isNotEmpty())
            {
                DBG("Plugin Load Failed: " << error);  // Log the error message
                auto options = MessageBoxOptions::makeOptionsOk(MessageBoxIconType::WarningIcon,
                                                                "Plugin Load Failed",
                                                                error);
                messageBox = AlertWindow::showScopedAsync(options, nullptr);
                return;
            }
    
            inner = std::move(instance);
            editorStyle = where;
    
            if (inner != nullptr && !mb.isEmpty())
                inner->setStateInformation(mb.getData(), (int)mb.getSize());
    
            if (active)
            {
                inner->setRateAndBufferSizeDetails(getSampleRate(), getBlockSize());
                inner->prepareToPlay(getSampleRate(), getBlockSize());
            }
    
            NullCheckedInvocation::invoke(pluginChanged);
        };
    
        DBG("Loading plugin: " << pd.name << " | Identifier: " << pd.fileOrIdentifier);  // Log the plugin being loaded
        pluginFormatManager.createPluginInstanceAsync(pd, getSampleRate(), getBlockSize(), callback);
    }
    void clearPlugin()
    {
        const ScopedLock sl (innerMutex);

        inner = nullptr;
        NullCheckedInvocation::invoke (pluginChanged);
    }

    bool isPluginLoaded() const
    {
        const ScopedLock sl (innerMutex);
        return inner != nullptr;
    }

    std::unique_ptr<AudioProcessorEditor> createInnerEditor() const
    {
        const ScopedLock sl (innerMutex);
        return rawToUniquePtr (inner->hasEditor() ? inner->createEditorIfNeeded() : nullptr);
    }

    EditorStyle getEditorStyle() const noexcept { return editorStyle; }

    ApplicationProperties appProperties;
    AudioPluginFormatManager pluginFormatManager;
    KnownPluginList pluginList;
    std::function<void()> pluginChanged;

private:
    CriticalSection innerMutex;
    std::unique_ptr<AudioPluginInstance> inner;
    EditorStyle editorStyle = EditorStyle{};
    bool active = false;
    ScopedMessageBox messageBox;

    static constexpr const char* innerStateTag = "inner_state";
    static constexpr const char* editorStyleTag = "editor_style";

    void changeListenerCallback (ChangeBroadcaster* source) final
    {
        if (source != &pluginList)
            return;

        if (auto savedPluginList = pluginList.createXml())
        {
            appProperties.getUserSettings()->setValue ("pluginList", savedPluginList.get());
            appProperties.saveIfNeeded();
        }
    }
};

//==============================================================================
constexpr auto margin = 10;

static void doLayout (Component* main, Component& bottom, int bottomHeight, Rectangle<int> bounds)
{
    Grid grid;
    grid.setGap (Grid::Px { margin });
    grid.templateColumns = { Grid::TrackInfo { Grid::Fr { 1 } } };
    grid.templateRows = { Grid::TrackInfo { Grid::Fr { 1 } },
                          Grid::TrackInfo { Grid::Px { bottomHeight }} };
    grid.items = { GridItem { main }, GridItem { bottom }.withMargin ({ 0, margin, margin, margin }) };
    grid.performLayout (bounds);
}

class PluginLoaderComponent final : public Component
{
public:
    template <typename Callback>
    PluginLoaderComponent (AudioPluginFormatManager& manager,
                           KnownPluginList& list,
                           Callback&& callback)
        : pluginListComponent (manager, list, {}, {})
    {
        pluginListComponent.getTableListBox().setMultipleSelectionEnabled (false);

        addAndMakeVisible (pluginListComponent);
        addAndMakeVisible (addButton);

        addButton.onClick = [this, &list, cb = std::forward<Callback> (callback)]
        {
            const auto index = pluginListComponent.getTableListBox().getSelectedRow();
            const auto& types = list.getTypes();

            if (isPositiveAndBelow (index, types.size()))
                NullCheckedInvocation::invoke (cb, types.getReference (index));
        };
    }

    void resized() override
    {
        auto area = getLocalBounds();
        pluginListComponent.setBounds(area.removeFromTop(area.getHeight() - 40));
        addButton.setBounds(area);
    }

private:
    PluginListComponent pluginListComponent;
    TextButton addButton { "Add to Chain" };
};

//==============================================================================
class PluginEditorComponent final : public Component
{
public:
    template <typename Callback>
    PluginEditorComponent (std::unique_ptr<AudioProcessorEditor> editorIn, Callback&& onClose)
        : editor (std::move (editorIn))
    {
        addAndMakeVisible (editor.get());
        addAndMakeVisible (closeButton);

        childBoundsChanged (editor.get());

        closeButton.onClick = std::forward<Callback> (onClose);
    }

    void setScaleFactor (float scale)
    {
        if (editor != nullptr)
            editor->setScaleFactor (scale);
    }

    void resized() override
    {
        doLayout (editor.get(), closeButton, buttonHeight, getLocalBounds());
    }

    void childBoundsChanged (Component* child) override
    {
        if (child != editor.get())
            return;

        const auto size = editor != nullptr ? editor->getLocalBounds()
                                            : Rectangle<int>();

        setSize (size.getWidth(), margin + buttonHeight + size.getHeight());
    }

private:
    static constexpr auto buttonHeight = 40;

    std::unique_ptr<AudioProcessorEditor> editor;
    TextButton closeButton { "Close Plugin" };
};

//==============================================================================
class ScaledDocumentWindow final : public DocumentWindow
{
public:
    ScaledDocumentWindow (Colour bg, float scale)
        : DocumentWindow ("Editor", bg, 0), desktopScale (scale) {}

    float getDesktopScaleFactor() const override { return Desktop::getInstance().getGlobalScaleFactor() * desktopScale; }

private:
    float desktopScale = 1.0f;
};

//==============================================================================
class EffectsChainComponent : public Component
{
public:
    EffectsChainComponent(HostAudioProcessorImpl& processor)
        : hostProcessor(processor)
    {
        addAndMakeVisible(addButton);
        addButton.onClick = [this] { openPluginDialog(); };
    }

    void resized() override
    {
        auto area = getLocalBounds();
        addButton.setBounds(area.removeFromRight(50).reduced(5));
        for (auto* pluginButton : pluginButtons)
            pluginButton->setBounds(area.removeFromLeft(100).reduced(5));
    }

    void addPlugin(const PluginDescription& pd)
    {
        auto* pluginButton = new TextButton(pd.name);
        pluginButtons.add(pluginButton);
        addAndMakeVisible(pluginButton);
        resized();
    }

private:
    void openPluginDialog()
    {
        DBG("Opening plugin dialog...");

        auto* pluginLoader = new PluginLoaderComponent(hostProcessor.pluginFormatManager, hostProcessor.pluginList, 
            [this](const PluginDescription& pd)
            {
                hostProcessor.setNewPlugin(pd, EditorStyle::thisWindow);
                addPlugin(pd);
                if (auto* dialog = DialogWindow::getCurrentlyModalComponent())
                {
                    dialog->exitModalState(0);
                }
            });

        pluginLoader->setSize(400, 300); // Set a reasonable size for the dialog content

        DialogWindow::LaunchOptions options;
        options.content.setOwned(pluginLoader);
        options.dialogTitle = "Select Plugin";
        options.dialogBackgroundColour = Colours::lightgrey;
        options.escapeKeyTriggersCloseButton = true;
        options.useNativeTitleBar = true;
        options.resizable = true;
        options.launchAsync();

        DBG("Plugin dialog launched.");
    }

    HostAudioProcessorImpl& hostProcessor;
    TextButton addButton { "+" };
    OwnedArray<TextButton> pluginButtons;
};

//==============================================================================
class HostAudioProcessorEditor final : public AudioProcessorEditor
{
public:
    explicit HostAudioProcessorEditor (HostAudioProcessorImpl& owner)
        : AudioProcessorEditor (owner),
          hostProcessor (owner),
          scopedCallback (owner.pluginChanged, [this] { pluginChanged(); }),
          effectsChain(owner)
    {
        setSize (500, 500);
        setResizable (false, false);
        addAndMakeVisible (closeButton);
        addAndMakeVisible (effectsChain);

        hostProcessor.pluginChanged();

        closeButton.onClick = [this] { clearPlugin(); };
    }

    void paint (Graphics& g) override
    {
        g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId).darker());
    }

    void resized() override
    {
        auto area = getLocalBounds();
        effectsChain.setBounds(area.removeFromTop(50));
        closeButton.setBounds (area.removeFromBottom(30).withSizeKeepingCentre (200, buttonHeight));
    }

    void childBoundsChanged (Component* child) override
    {
        if (child != editor.get())
            return;

        const auto size = editor != nullptr ? editor->getLocalBounds()
                                            : Rectangle<int>();

        setSize (size.getWidth(), size.getHeight());
    }

    void setScaleFactor (float scale) override
    {
        currentScaleFactor = scale;
        AudioProcessorEditor::setScaleFactor (scale);

        [[maybe_unused]] const auto posted = MessageManager::callAsync ([ref = SafePointer<HostAudioProcessorEditor> (this), scale]
        {
            if (auto* r = ref.getComponent())
                if (auto* e = r->currentEditorComponent)
                    e->setScaleFactor (scale);
        });

        jassert (posted);
    }

private:
    void pluginChanged()
    {
        closeButton.setVisible (hostProcessor.isPluginLoaded());

        if (hostProcessor.isPluginLoaded())
        {
            auto editorComponent = std::make_unique<PluginEditorComponent> (hostProcessor.createInnerEditor(), [this]
            {
                [[maybe_unused]] const auto posted = MessageManager::callAsync ([this] { clearPlugin(); });
                jassert (posted);
            });

            editorComponent->setScaleFactor (currentScaleFactor);
            currentEditorComponent = editorComponent.get();

            editor = [&]() -> std::unique_ptr<Component>
            {
                switch (hostProcessor.getEditorStyle())
                {
                    case EditorStyle::thisWindow:
                        addAndMakeVisible (editorComponent.get());
                        setSize (editorComponent->getWidth(), editorComponent->getHeight());
                        return std::move (editorComponent);

                    case EditorStyle::newWindow:
                        const auto bg = getLookAndFeel().findColour (ResizableWindow::backgroundColourId).darker();
                        auto window = std::make_unique<ScaledDocumentWindow> (bg, currentScaleFactor);
                        window->setAlwaysOnTop (true);
                        window->setContentOwned (editorComponent.release(), true);
                        window->centreAroundComponent (this, window->getWidth(), window->getHeight());
                        window->setVisible (true);
                        return window;
                }

                jassertfalse;
                return nullptr;
            }();
        }
        else
        {
            editor = nullptr;
            setSize (500, 500);
        }
    }

    void clearPlugin()
    {
        currentEditorComponent = nullptr;
        editor = nullptr;
        hostProcessor.clearPlugin();
    }

    static constexpr auto buttonHeight = 30;

    HostAudioProcessorImpl& hostProcessor;
    std::unique_ptr<Component> editor;
    PluginEditorComponent* currentEditorComponent = nullptr;
    ScopedValueSetter<std::function<void()>> scopedCallback;
    TextButton closeButton { "Close Plugin" };
    EffectsChainComponent effectsChain;
    float currentScaleFactor = 1.0f;
};

//==============================================================================

class HostAudioProcessor final : public HostAudioProcessorImpl
{
public:
    bool hasEditor() const override { return true; }
    AudioProcessorEditor* createEditor() override { return new HostAudioProcessorEditor (*this); }
};
