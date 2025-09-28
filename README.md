# Battery Status

Shows and display your device's battery status in game.

- <cy>Shows your device battery percentage</c>
- <cy>Change the battery status display (icon or percentage)</c>
- <cy>Change the battery status position (top left, top right, bottom left, bottom right)</c>
- <cy>Adjust the battery threshold warning level</c>

<co>You can change all of these settings in the mod settings.</c>

### Colors Based on Battery Percentage

- Red <cr>(Low Battery)</cg> - Below Low Battery
- Orange <co>(Medium Battery)</cg> - Between Low and Medium Battery
- Yellow <cy>(High Battery)</cg> - Between Medium and High Battery
- Green <cg>(Full Battery)</cg> - Above High Battery
- Gray <cg>(Unknown)</cg> - When the battery status is unknown
- 
### Supported Devices

- iOS <cg>(Battery Percentage shows in 5% increments)</cg>
- macOS
- Windows <cg>(Battery Saver doesn't work on Windows 11)</cg>

### Why is Android support isn't added?

<cr>Adding android support is super duper complex compared to other devices. This involves having to tinker coco2dx jnihelper and all of those stuff that I can't be bothered to do. Even testing it intensively I still couldn't get it working :/</c>
<cr>If anyone wants to implement it, feel free to fork the repo and make a pr.</c>

![king](arcticwoof.twitch_interactive/king.png)