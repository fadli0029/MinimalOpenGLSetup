### TODO's
- [x] FPS Camera system
- [x] Arcball Camera system
- [x] Code cleanup
- [x] Functionality to switch between different xcamera systems (via some kind of button?)
- [x] Currently after switching camera, the position resets. This is not a desired behavior. If I switch camera after having moved to a certain position while I was in FPS camera, then the camera should switch to arcball on the spot and not move me back to where I was.
- [ ] Figure out better way to display the cube? (not that important now)
- [ ] Integrate Bullet3 physics
- [ ] More code cleanup


### Using Glad
This setup uses Glad to include OpenGL functionalities: https://glad.dav1d.de/

For the settings, see the image below. Click `ADD ALL` after selecting `API->gl->Version 4.1`.

Then, after generating the necessary files, extract the zip file and paste the `glad` folder and `KHR` folder into the `include` directory. Put the `glad.c` file into the `src` directory. That's it! The CMakeLists.txt file is already set up to work with this setup.

![Project Logo](glad_settings.png)
