
    #include "Wrapper.h"

    Wrapper::Wrapper(): initialized_resources_(false), has_focus_(false), app_(NULL), controller(NULL), has_video(false), has_context(false)
    {
        gl_context_ = ndk_helper::GLContext::GetInstance();
        mtpipe = mtpipe_alloc(10);
    }

    Wrapper::~Wrapper() { }

    /* initialize opengl context and controller */

    void Wrapper::InitController( )
    {
        AConfiguration* config = AConfiguration_new();
        AConfiguration_fromAssetManager(config, app_->activity->assetManager);
        int32_t density = AConfiguration_getDensity(config);
        AConfiguration_delete(config);

        if ( controller == NULL )
        {
            environment_t env =
            {
                    .width = (float) gl_context_->GetScreenWidth(),
                    .height = (float) gl_context_->GetScreenHeight(),
                    .scale = (float) (density / 160.0),
                    .windowed = 0,
                    .path_library =  mtcstr_fromcstring(
                            (char *) app_->activity->internalDataPath),
                    .path_resources = mtcstr_fromcstring(
                            (char *) app_->activity->internalDataPath),
                    .defaultFrameBuffer = 0,
                    .defaultRenderBuffer = 0
            };

            controller = controller_alloc(env);
            gl_context_->Swap();
        }
    }

    int Wrapper::InitDisplay( )
    {
        if (!initialized_resources_)
        {
            gl_context_->Init(app_->window);
            initialized_resources_ = true;
            glClearColor(1.0,1.0,1.0,1.0);
            glClear(GL_COLOR_BUFFER_BIT);
            gl_context_->Swap();
            InitController( );
            has_context = true;
        }
        else
        {
            if (EGL_SUCCESS != gl_context_->Resume(app_->window))
            {
                mtmem_release( controller );
                InitController();
            }
            gl_context_->Swap( );
            has_context = true;
        }

        return 0;
    }
    /* Tear down the EGL context currently associated with the display. */

    void Wrapper::TermDisplay()
    {
        gl_context_->Suspend();
    }

    /* release caches */

    void Wrapper::TrimMemory() { }

    /* Just the current frame in the display. */

    void Wrapper::DrawFrame() {

        struct timeval now;
        gettimeofday( &now , NULL );

        if ( has_video )
        {
            JNIEnv* jni;
            app_->activity->vm->AttachCurrentThread(&jni, NULL);
            jclass clazz = jni->GetObjectClass(app_->activity->clazz);
            jmethodID methodID = jni->GetMethodID(clazz, "renderVideoToTexture", "()I");
            has_video = jni->CallIntMethod(app_->activity->clazz, methodID);
            jni->DeleteLocalRef( clazz );
            app_->activity->vm->DetachCurrentThread();
        }

        char render = 0;

        input_t* keyinput = ( input_t* ) mtpipe_recv( mtpipe );

        if ( keyinput != NULL )
        {
            controller_input( controller , keyinput );
            render = keyinput->render;
            mtmem_release( keyinput->stringa );
            mtmem_release( keyinput );
        }

        input_t input = { 0 };
        input.type = kInputTypeTimer;
        controller_input( controller , &input );

        render |= input.render;

        if ( render == 1 ) gl_context_->Swap();
    }

    /* key down from ui thread */

    void Wrapper::OnKeyDown(const char* chars, int keycode)
    {
        input_t* input = (input_t*) mtmem_calloc( sizeof( input_t ) , NULL );
        input->type = kInputTypeKeyPress;
        input->stringa = mtcstr_fromcstring( ( char* ) chars );

        if ( keycode == AKEYCODE_ENTER ) input->key = kInputKeyTypeReturn;
        else if ( keycode == AKEYCODE_DEL ) input->key = kInputKeyTypeBackspace;
        else input->key = kInputKeyTypeNormal;

        mtpipe_send( mtpipe , input );
    }

    /* queue input */

    void Wrapper::QueueInput( input_t* input )
    {
        input_t* oneinput = (input_t*) mtmem_calloc( sizeof( input_t ) , NULL );
        memcpy( oneinput , input , sizeof( input_t ));
        oneinput->stringa = mtcstr_fromcstring( ( char* ) input->stringa );
        mtpipe_send( mtpipe , oneinput );
    }

    /* show keyboard from ui thread */

    void Wrapper::ShowKeyboard() {
        input_t input = { 0 };
        input.type = kInputTypeVisibleFrameChanged;
        input.floata = ( float ) gl_context_->GetScreenWidth();
        input.floatb = ( float ) gl_context_->GetScreenHeight()/2;

        controller_input( controller , &input );
        if ( input.render == 1 ) gl_context_->Swap();
    }

    /* hide keyboard from ui thread */

    void Wrapper::HideKeyboard() {
        input_t input = { 0 };
        input.type = kInputTypeVisibleFrameChanged;
        input.floata = ( float ) gl_context_->GetScreenWidth();
        input.floatb = ( float ) gl_context_->GetScreenHeight();

        controller_input( controller , &input );
        if ( input.render == 1 ) gl_context_->Swap();
    }

    /* Process the next input event. */

    int32_t Wrapper::HandleInput(android_app* app, AInputEvent* event)
    {
        Wrapper* eng = (Wrapper*) app->userData;
        controller_t* controller = eng->controller;

        if ( controller != NULL )
        {
            ndk_helper::GLContext *gl_context_ = eng->gl_context_;
            if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
            {
                int32_t action = AMotionEvent_getAction(event);
                uint32_t flags = action & AMOTION_EVENT_ACTION_MASK;

                int32_t count = AMotionEvent_getPointerCount(event);
                int32_t iIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)
                                 >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
                int32_t pointerId = AMotionEvent_getPointerId(event, iIndex);

                char *ids[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8"};

                float x = AMotionEvent_getX(event, pointerId);
                float y = AMotionEvent_getY(event, pointerId);

                input_t input = { 0 };
                input.stringa = ids[pointerId];
                input.floata = x;
                input.floatb = y;

                switch (flags)
                {
                    case AMOTION_EVENT_ACTION_DOWN : input.type = kInputTypeTouchDown; break;
                    case AMOTION_EVENT_ACTION_POINTER_DOWN : input.type = kInputTypeTouchDown; break;
                    case AMOTION_EVENT_ACTION_UP : input.type = kInputTypeTouchUp; break;
                    case AMOTION_EVENT_ACTION_POINTER_UP: input.type = kInputTypeTouchUp; break;
                    case AMOTION_EVENT_ACTION_MOVE : input.type = kInputTypeTouchDrag; break;
                    case AMOTION_EVENT_ACTION_CANCEL: input.type = kInputTypeTouchUp; break;
                }

                controller_input( controller , &input );
                if ( input.render == 1 ) gl_context_->Swap();

                return 1;
            }
            else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY)
            {
                int32_t keycode = AKeyEvent_getKeyCode(event);
                input_t input = {0};
                input.type = kInputTypeKeyPress;
                input.stringa = NULL;

                if ( keycode == AKEYCODE_ENTER ) input.key = kInputKeyTypeReturn;
                if ( keycode == AKEYCODE_DEL ) input.key = kInputKeyTypeBackspace;

                if ( AKeyEvent_getAction(event) == AKEY_EVENT_ACTION_DOWN ) input.type = kInputTypeKeyPress;
                if ( AKeyEvent_getAction(event) == AKEY_EVENT_ACTION_UP ) input.type = kInputTypeKeyRelease;

                controller_input( controller , &input );
                if ( input.render == 1 ) gl_context_->Swap();
            }
        }
        return 0;
    }

    /* Process the next main command. */

    void Wrapper::HandleCmd(struct android_app* app, int32_t cmd)
    {
        Wrapper* eng = (Wrapper*)app->userData;

        switch (cmd)
        {
            case APP_CMD_SAVE_STATE:
                break;
            case APP_CMD_INIT_WINDOW:
                // The window is being shown, get it ready.
                if (app->window != NULL) {
                    eng->InitDisplay();
                    input_t input = { 0 };
                    input.type = kInputTypeEnterForeground;
                    if ( eng->controller != NULL ) controller_input( eng->controller , &input );
                }

                break;
            case APP_CMD_TERM_WINDOW:
            {
                // The window is being hidden or closed, clean it up.
                input_t input = { 0 };
                input.type = kInputTypeEnterBackground;
                if ( eng->controller != NULL ) controller_input( eng->controller , &input );
                eng->TermDisplay();
                eng->has_focus_ = false;
                eng->has_context = false;
                break;
            }
            case APP_CMD_STOP:
                break;
            case APP_CMD_GAINED_FOCUS:
                // Start animation
                eng->has_focus_ = true;
                break;
            case APP_CMD_LOST_FOCUS:
                // Also stop animating.
                eng->has_focus_ = false;
                break;
            case APP_CMD_LOW_MEMORY:
                // Free up GL resources
                eng->TrimMemory();
                break;
        }
    }

    void Wrapper::SetState(android_app* state) {
        app_ = state;
    }

    bool Wrapper::IsReady() {
        if (has_focus_ && has_context) return true;
        return false;
    }
