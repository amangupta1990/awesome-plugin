#include <AVFoundation/AVFoundation.h>
#include <juce_audio_processors/juce_audio_processors.h>

void scanForPlugins(juce::KnownPluginList& pluginList)
{
    DBG("🔍 Scanning for AUv3 plugins...");

    AVAudioUnitComponentManager* manager = [AVAudioUnitComponentManager sharedAudioUnitComponentManager];

    NSPredicate* predicate = [NSPredicate predicateWithValue:YES];
    NSArray<AVAudioUnitComponent*>* components = [manager componentsMatchingPredicate:predicate];

    if (components.count == 0)
    {
        DBG("⚠️ No third-party AUv3 plugins found!");
        return;
    }

    DBG("✅ Found " << components.count << " AUv3 Plugins:");

    for (AVAudioUnitComponent* component in components)
    {
        AudioComponentDescription desc = [component audioComponentDescription];

        juce::PluginDescription pluginDescription;
        pluginDescription.name = [component name].UTF8String;
        pluginDescription.pluginFormatName = "AudioUnit";

        // ✅ Convert Type, Subtype, and Manufacturer to 4-character codes
        auto uint32ToFourCC = [](uint32_t code) -> juce::String
        {
            return juce::String::charToString((code >> 24) & 0xFF) +
                   juce::String::charToString((code >> 16) & 0xFF) +
                   juce::String::charToString((code >> 8) & 0xFF) +
                   juce::String::charToString((code) & 0xFF);
        };

        juce::String category = juce::String([component typeName].UTF8String).replaceCharacters(" /", "__");
        juce::String type = uint32ToFourCC(desc.componentType);
        juce::String subtype = uint32ToFourCC(desc.componentSubType);
        juce::String manufacturer = uint32ToFourCC(desc.componentManufacturer);

        // ✅ Correct AUv3 identifier format
        pluginDescription.fileOrIdentifier = "AudioUnit:" + category + "/" + type + "," + subtype + "," + manufacturer;

        pluginDescription.category = [component typeName].UTF8String;
        pluginDescription.manufacturerName = [component manufacturerName].UTF8String;
        pluginDescription.version = [component versionString].UTF8String;
        pluginDescription.uniqueId = desc.componentSubType;

        pluginList.addType(pluginDescription);

        DBG("🎵 Plugin: " << pluginDescription.name
            << " | Identifier: " << pluginDescription.fileOrIdentifier  // ✅ Correctly formatted identifier
            << " | Type: " << type
            << " | Subtype: " << subtype
            << " | Manufacturer: " << manufacturer);
    }

    DBG("✅ Finished scanning for plugins!");
}
