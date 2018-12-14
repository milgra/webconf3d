
    #ifndef ANDROID_ENGINE_H
    #define ANDROID_ENGINE_H

    #include <jni.h>
    #include <android/sensor.h>
    #include <android_native_app_glue.h>
    #include <GLContext.h>

    extern "C"
    {
        #include "controller.h"
        #include "../../../framework/core/mtpipe.h"
        #include "../../../framework/core/mtcstr.h"
    }

    class Wrapper
    {
        bool initialized_resources_;
        bool has_focus_;
        bool has_context;
        mtpipe_t* mtpipe;

    public:

        bool has_video;

        static void HandleCmd(struct android_app* app, int32_t cmd);
        static int32_t HandleInput(android_app* app, AInputEvent* event);

        android_app* app_;
        ndk_helper::GLContext* gl_context_;
        controller_t* controller;

        Wrapper();
        ~Wrapper();

        void SetState(android_app* state);
        int InitDisplay();
        void InitController();
        void DrawFrame();
        void TermDisplay();
        void TrimMemory();
        bool IsReady();
        void ShowKeyboard();
        void HideKeyboard();
        void OnKeyDown(const char* string, int keycode);
        void QueueInput( input_t* input );
    };

    #endif
