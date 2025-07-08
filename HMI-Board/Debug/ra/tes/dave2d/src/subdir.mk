################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ra/tes/dave2d/src/dave_64bitoperation.c \
../ra/tes/dave2d/src/dave_base.c \
../ra/tes/dave2d/src/dave_blit.c \
../ra/tes/dave2d/src/dave_box.c \
../ra/tes/dave2d/src/dave_circle.c \
../ra/tes/dave2d/src/dave_context.c \
../ra/tes/dave2d/src/dave_curve.c \
../ra/tes/dave2d/src/dave_dlist.c \
../ra/tes/dave2d/src/dave_driver.c \
../ra/tes/dave2d/src/dave_edge.c \
../ra/tes/dave2d/src/dave_errorcodes.c \
../ra/tes/dave2d/src/dave_gradient.c \
../ra/tes/dave2d/src/dave_hardware.c \
../ra/tes/dave2d/src/dave_line.c \
../ra/tes/dave2d/src/dave_math.c \
../ra/tes/dave2d/src/dave_memory.c \
../ra/tes/dave2d/src/dave_pattern.c \
../ra/tes/dave2d/src/dave_perfcount.c \
../ra/tes/dave2d/src/dave_polyline.c \
../ra/tes/dave2d/src/dave_quad.c \
../ra/tes/dave2d/src/dave_rbuffer.c \
../ra/tes/dave2d/src/dave_render.c \
../ra/tes/dave2d/src/dave_texture.c \
../ra/tes/dave2d/src/dave_triangle.c \
../ra/tes/dave2d/src/dave_utility.c \
../ra/tes/dave2d/src/dave_viewport.c \
../ra/tes/dave2d/src/dave_wedge.c 

OBJS += \
./ra/tes/dave2d/src/dave_64bitoperation.o \
./ra/tes/dave2d/src/dave_base.o \
./ra/tes/dave2d/src/dave_blit.o \
./ra/tes/dave2d/src/dave_box.o \
./ra/tes/dave2d/src/dave_circle.o \
./ra/tes/dave2d/src/dave_context.o \
./ra/tes/dave2d/src/dave_curve.o \
./ra/tes/dave2d/src/dave_dlist.o \
./ra/tes/dave2d/src/dave_driver.o \
./ra/tes/dave2d/src/dave_edge.o \
./ra/tes/dave2d/src/dave_errorcodes.o \
./ra/tes/dave2d/src/dave_gradient.o \
./ra/tes/dave2d/src/dave_hardware.o \
./ra/tes/dave2d/src/dave_line.o \
./ra/tes/dave2d/src/dave_math.o \
./ra/tes/dave2d/src/dave_memory.o \
./ra/tes/dave2d/src/dave_pattern.o \
./ra/tes/dave2d/src/dave_perfcount.o \
./ra/tes/dave2d/src/dave_polyline.o \
./ra/tes/dave2d/src/dave_quad.o \
./ra/tes/dave2d/src/dave_rbuffer.o \
./ra/tes/dave2d/src/dave_render.o \
./ra/tes/dave2d/src/dave_texture.o \
./ra/tes/dave2d/src/dave_triangle.o \
./ra/tes/dave2d/src/dave_utility.o \
./ra/tes/dave2d/src/dave_viewport.o \
./ra/tes/dave2d/src/dave_wedge.o 

C_DEPS += \
./ra/tes/dave2d/src/dave_64bitoperation.d \
./ra/tes/dave2d/src/dave_base.d \
./ra/tes/dave2d/src/dave_blit.d \
./ra/tes/dave2d/src/dave_box.d \
./ra/tes/dave2d/src/dave_circle.d \
./ra/tes/dave2d/src/dave_context.d \
./ra/tes/dave2d/src/dave_curve.d \
./ra/tes/dave2d/src/dave_dlist.d \
./ra/tes/dave2d/src/dave_driver.d \
./ra/tes/dave2d/src/dave_edge.d \
./ra/tes/dave2d/src/dave_errorcodes.d \
./ra/tes/dave2d/src/dave_gradient.d \
./ra/tes/dave2d/src/dave_hardware.d \
./ra/tes/dave2d/src/dave_line.d \
./ra/tes/dave2d/src/dave_math.d \
./ra/tes/dave2d/src/dave_memory.d \
./ra/tes/dave2d/src/dave_pattern.d \
./ra/tes/dave2d/src/dave_perfcount.d \
./ra/tes/dave2d/src/dave_polyline.d \
./ra/tes/dave2d/src/dave_quad.d \
./ra/tes/dave2d/src/dave_rbuffer.d \
./ra/tes/dave2d/src/dave_render.d \
./ra/tes/dave2d/src/dave_texture.d \
./ra/tes/dave2d/src/dave_triangle.d \
./ra/tes/dave2d/src/dave_utility.d \
./ra/tes/dave2d/src/dave_viewport.d \
./ra/tes/dave2d/src/dave_wedge.d 


# Each subdirectory must supply rules for building sources it contributes
ra/tes/dave2d/src/%.o: ../ra/tes/dave2d/src/%.c
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O3 -ffunction-sections -fdata-sections -Wall  -g -gdwarf-2 -I"D:\RT-ThreadStudio\workspace\hmi-demo3\src" -I"D:\RT-ThreadStudio\workspace\hmi-demo3" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\board\lvgl" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\board\ports\touch\gt911\inc" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\board\ports\wifi" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\board\ports" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\board" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\libraries\HAL_Drivers\config" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\libraries\HAL_Drivers" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\env_support\rt-thread" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\core" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\draw\arm2d" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\draw\nxp\pxp" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\draw\nxp\vglite" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\draw\renesas" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\draw\sdl" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\draw\stm32_dma2d" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\draw\swm341_dma2d" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\draw\sw" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\draw" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\layouts\flex" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\layouts\grid" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\layouts" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\libs\bmp" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\libs\ffmpeg" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\libs\freetype" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\libs\fsdrv" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\libs\gif" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\libs\png" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\libs\qrcode" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\libs\rlottie" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\libs\sjpg" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\libs\tiny_ttf" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\libs" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\others\fragment" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\others\gridnav" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\others\ime" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\others\imgfont" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\others\monkey" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\others\msg" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\others\snapshot" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\others" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\themes\basic" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\themes\default" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\themes\mono" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\themes" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\widgets\animimg" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\widgets\calendar" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\widgets\chart" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\widgets\colorwheel" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\widgets\imgbtn" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\widgets\keyboard" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\widgets\led" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\widgets\list" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\widgets\menu" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\widgets\meter" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\widgets\msgbox" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\widgets\span" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\widgets\spinbox" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\widgets\spinner" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\widgets\tabview" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\widgets\tileview" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\widgets\win" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra\widgets" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\extra" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\font" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\hal" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\misc" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src\widgets" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\LVGL-v8.3.11\src" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\cJSON-v1.7.17" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\llmchat-latest\ports" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\llmchat-latest" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\mbedtls-v2.28.1\mbedtls\include" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\mbedtls-v2.28.1\mbedtls\library" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\mbedtls-v2.28.1\ports\inc" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\rw007-v2.1.0\inc" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\rw007-v2.1.0" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\packages\webclient-v2.2.0\inc" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\ra\arm\CMSIS_5\CMSIS\Core\Include" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\ra\fsp\inc\api" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\ra\fsp\inc\instances" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\ra\fsp\inc" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\ra\tes\dave2d\inc" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\ra_cfg\fsp_cfg\bsp" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\ra_cfg\fsp_cfg" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\ra_gen" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\rt-thread\components\dfs\dfs_v1\filesystems\devfs" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\rt-thread\components\dfs\dfs_v1\filesystems\elmfat" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\rt-thread\components\dfs\dfs_v1\include" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\rt-thread\components\drivers\include" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\rt-thread\components\drivers\spi" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\rt-thread\components\drivers\touch" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\rt-thread\components\drivers\wlan" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\rt-thread\components\finsh" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\rt-thread\components\libc\compilers\common\include" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\rt-thread\components\libc\compilers\newlib" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\rt-thread\components\libc\posix\io\poll" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\rt-thread\components\libc\posix\io\stdio" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\rt-thread\components\libc\posix\ipc" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\rt-thread\components\net\lwip\lwip-2.0.3\src\include\netif" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\rt-thread\components\net\lwip\lwip-2.0.3\src\include" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\rt-thread\components\net\lwip\port" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\rt-thread\components\net\netdev\include" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\rt-thread\components\net\sal\impl" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\rt-thread\components\net\sal\include\dfs_net" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\rt-thread\components\net\sal\include\socket\sys_socket" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\rt-thread\components\net\sal\include\socket" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\rt-thread\components\net\sal\include" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\rt-thread\include" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\rt-thread\libcpu\arm\common" -I"D:\RT-ThreadStudio\workspace\hmi-demo3\rt-thread\libcpu\arm\cortex-m4" -include"D:\RT-ThreadStudio\workspace\hmi-demo3\rtconfig_preinc.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

