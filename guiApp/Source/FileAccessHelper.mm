#import <Cocoa/Cocoa.h>
#import <juce_core/juce_core.h>

bool requestAccessToFolder(juce::File &selectedFolder)
{
    @autoreleasepool
    {
        NSOpenPanel *panel = [NSOpenPanel openPanel];
        [panel setCanChooseFiles:NO];
        [panel setCanChooseDirectories:YES];
        [panel setAllowsMultipleSelection:NO];
        [panel setDirectoryURL:[NSURL fileURLWithPath:[NSString stringWithUTF8String:selectedFolder.getFullPathName().toUTF8()]]];

        if ([panel runModal] == NSModalResponseOK)
        {
            NSURL *url = [panel URL];
            selectedFolder = juce::File(juce::String([url.path UTF8String]));
            return true;
        }
        else
        {
            return false;
        }
    }
}