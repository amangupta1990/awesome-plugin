// PluginScanner.mm
#include <AVFoundation/AVFoundation.h>
#include <iostream>
#include "hostPluginDemo.h"

void scanForPlugins()
{
    std::cout << "🔍 Scanning for AUv3 plugins..." << std::endl;

    AVAudioUnitComponentManager* manager = [AVAudioUnitComponentManager sharedAudioUnitComponentManager];
    NSArray<AVAudioUnitComponent*>* components = [manager componentsMatchingPredicate:nil];

    if (components.count == 0)
    {
        std::cout << "⚠️ No third-party AUv3 plugins found!" << std::endl;
        return;
    }

    std::cout << "✅ Found " << components.count << " AUv3 Plugins:" << std::endl;

    for (AVAudioUnitComponent* component in components)
    {
        std::cout << "🎵 Plugin: " << [component name].UTF8String
                  << " | Identifier: " << [component componentDescription].componentSubType
                  << std::endl;
    }

    std::cout << "✅ Finished scanning for plugins!" << std::endl;
}
