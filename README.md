### Using Glad
This setup uses Glad to include OpenGL functionalities: https://glad.dav1d.de/

For the settings, see the image below. Click `ADD ALL` after selecting `API->gl->Version 4.1`.

Then, after generating the necessary files, extract the zip file and paste the `glad` folder and `KHR` folder into the `include` directory. Put the `glad.c` file into the `src` directory. That's it! The CMakeLists.txt file is already set up to work with this setup.

<img src="glad_settings.png" alt="GLAD settings" width="300" height="200">
