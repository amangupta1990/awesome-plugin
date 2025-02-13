#include <AVFoundation/AVFoundation.h>
#include <iostream>

void scanForPlugins()
{
    std::cout << "🔍 Scanning for AUv3 plugins..." << std::endl;

    AVAudioUnitComponentManager* manager = [AVAudioUnitComponentManager sharedAudioUnitComponentManager];

    // ✅ Use a valid predicate instead of nil
    NSPredicate* predicate = [NSPredicate predicateWithValue:YES];  
    NSArray<AVAudioUnitComponent*>* components = [manager componentsMatchingPredicate:predicate];

    if (components.count == 0)
    {
        std::cout << "⚠️ No third-party AUv3 plugins found!" << std::endl;
        return;
    }

    std::cout << "✅ Found " << components.count << " AUv3 Plugins:" << std::endl;

    for (AVAudioUnitComponent* component in components)
    {
        // ✅ Fix: Use `audioComponentDescription` instead of `componentDescription`
        AudioComponentDescription desc = [component audioComponentDescription];

        std::cout << "🎵 Plugin: " << [component name].UTF8String
                  << " | Type: " << desc.componentType
                  << " | Subtype: " << desc.componentSubType
                  << " | Manufacturer: " << desc.componentManufacturer
                  << std::endl;
    }

    std::cout << "✅ Finished scanning for plugins!" << std::endl;
}
