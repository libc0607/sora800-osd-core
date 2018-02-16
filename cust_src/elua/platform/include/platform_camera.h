BOOL platform_camera_poweron(BOOL video_mode);


BOOL platform_camera_poweroff(void);


BOOL platform_camera_preview_open(u16 offsetx, u16 offsety,u16 startx, u16 starty, u16 endx, u16 endy);



BOOL platform_camera_preview_close(void);



BOOL platform_camera_capture(u16 width, u16 height);


BOOL platform_camera_save_photo(int iFd);

