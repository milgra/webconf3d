#include <jni.h>
    #include <android_native_app_glue.h>
    #include <GLContext.h>
    #include <Wrapper.h>

    extern "C"
    {
        #include "controller.h"
        #include "../../../framework/core/mtpipe.h"
        #include "../../../framework/core/mtcstr.h"
    }

    Wrapper wrapper;

    void android_main(android_app* state)
    {
      app_dummy();

      wrapper.SetState(state);

      state->userData = &wrapper;
      state->onAppCmd = Wrapper::HandleCmd;
      state->onInputEvent = Wrapper::HandleInput;

      while ( 1 )
      {
        int id;
        int events;
        android_poll_source* source;

        while ( 1 )
        {
          id = ALooper_pollAll( wrapper.IsReady() ? 0 : -1, NULL, &events, ( void** ) &source );

          if ( id < 0 ) break;
          if ( source != NULL ) source->process( state, source );
          if ( state->destroyRequested != 0 )
          {
            wrapper.TermDisplay();
            return;
          }
        }

        if ( wrapper.IsReady() )
        {
          wrapper.DrawFrame();
        }
      }
    }


extern "C"
{
/* key down event from java */

        void Java_com_milgra_kinetic_MyTextWatcher_onNativeKeyDown(JNIEnv* jni, jobject thiz, jstring string , int keycode )
        {
            const char *nativeString = jni->GetStringUTFChars(string, 0);
            wrapper.OnKeyDown( nativeString , keycode );
            jni->ReleaseStringUTFChars(string, nativeString);
        }


        void Java_com_milgra_kinetic_MediaHandler_onMediaHandlerOutput(JNIEnv *env, jobject instance,
                                                                       jstring name_, jstring path_,
                                                                       jint texture, jint width, jint height) {
            const char *name = env->GetStringUTFChars(name_, 0);
            const char *path = env->GetStringUTFChars(path_, 0);

            if ( strcmp( name , "medialoaded" ) == 0 )
            {
                input_t input = { 0 };
                input.type = kInputTypeMediaLoaded;
                input.inta = texture;
                input.floata = (float)width;
                input.floatb = (float)height;
                input.stringa = (char*) path;

                wrapper.QueueInput( &input );
            }
            env->ReleaseStringUTFChars(name_, name);
            env->ReleaseStringUTFChars(path_, path);
        }


        void Java_com_milgra_kinetic_VideoHandler_onMediaHandlerOutput(JNIEnv *env, jobject instance,
                                                                  jstring name_, jstring path_,
                                                                  jint texture, jint width,
                                                                  jint height) {
            const char *name = env->GetStringUTFChars(name_, 0);
            const char *path = env->GetStringUTFChars(path_, 0);

            if ( strcmp( name , "medialoaded" ) == 0 )
            {
                input_t input = { 0 };
                input.type = kInputTypeMediaLoaded;
                input.inta = texture;
                input.intb = 1;
                input.floata = (float)width;
                input.floatb = (float)height;
                input.stringa = (char*) path;

                wrapper.has_video = true;
                wrapper.QueueInput( &input );
            }

            env->ReleaseStringUTFChars(name_, name);
            env->ReleaseStringUTFChars(path_, path);
        }

        void Java_com_milgra_kinetic_NativeActivity_onNativeActivityOutput(JNIEnv *env, jobject instance,
                                                                           jstring name_, jstring path_,
                                                                           jint texture, jint width,
                                                                           jint height) {
            const char *name = env->GetStringUTFChars(name_, 0);
            const char *path = env->GetStringUTFChars(path_, 0);

            if ( strcmp( name , "framechanged" ) == 0 )
            {
                input_t input = { 0 };
                input.type = kInputTypeVisibleFrameChanged;
                input.floata = (float)width;
                input.floatb = (float)height;

                wrapper.QueueInput( &input );
            }

            env->ReleaseStringUTFChars(name_, name);
            env->ReleaseStringUTFChars(path_, path);
        }

        /* proxy output events to java */

        void controller_output( input_t* input )
        {
            JNIEnv* jni;
            wrapper.app_->activity->vm->AttachCurrentThread(&jni, NULL);

            jclass clazz = jni->GetObjectClass(wrapper.app_->activity->clazz);

            jstring jstr = jni->NewStringUTF( ( char* ) input->name );
            jstring url = jni->NewStringUTF( ( char* ) input->stringa );
            jstring adunitid = jni->NewStringUTF( ( char* ) input->stringb );
            jfloat volume = input->floata;
            jint texture = input->inta;
            jint loop = input->inta;

            jmethodID methodID = jni->GetMethodID(clazz, "onOutput", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;IFI)V");
            jni->CallVoidMethod(wrapper.app_->activity->clazz, methodID, jstr,url,adunitid,texture,volume,loop);

            //jni->ReleaseStringUTFChars( jstr , input->name );
            wrapper.app_->activity->vm->DetachCurrentThread();
        }
    }