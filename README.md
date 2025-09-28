# Battery Status

Shows and display your device's battery status in game.

- <cy>Shows your device battery percentage</c>
- <cy>Change the battery status display (icon or percentage)</c>
- <cy>Change the battery status position</c>
- <cy>Adjust the battery threshold warning level</c>

<co>You can change all of these settings in the mod settings.</c>

### Colors Based on Battery Percentage

- <cr>Red (Low Battery)</cr>: Below Low Battery
- <co>Orange (Medium Battery)</co>: Between Low and Medium Battery
- <cy>Yellow (High Battery)</cy>: Between Medium and High Battery
- <cg>Green (Full Battery)</cg>: Above High Battery
- <cl>Gray (Unknown)</cl>: When the battery status is unknown

### Supported Devices

- iOS <cj>(Battery Percentage shows in 5% increments)</cj>
- macOS
- Windows <cj>(Battery Saver doesn't work on Windows 11)</cj>

### Why is Android support isn't added?

<cr>Adding android support is super duper complex compared to other devices. This involves having to tinker coco2dx jnihelper and all of those stuff that I can't be bothered to do. Even testing it intensively I still couldn't get it working :/</c>

<cr>If anyone wants to implement it, feel free to fork the repo and make a pr.</c>

![king](arcticwoof.twitch_interactive/king.png)