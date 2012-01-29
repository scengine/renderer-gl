lib_LTLIBRARIES = libscerenderer.la

libscerenderer_la_CPPFLAGS  = -I$(srcdir)/../include
libscerenderer_la_CFLAGS    = @GLEW_CFLAGS@ \
                              @SCE_UTILS_CFLAGS@ \
                              @SCE_CORE_CFLAGS@ \
                              @PTHREAD_CFLAGS@ \
                              @GL_CFLAGS@ \
                              @SCE_DEBUG_CFLAGS@ \
                              @SCE_DEBUG_CFLAGS_EXPORT@
libscerenderer_la_LIBADD    = @GLEW_LIBS@ \
                              @SCE_UTILS_LIBS@ \
                              @SCE_CORE_LIBS@ \
                              @PTHREAD_LIBS@ \
                              @GL_LIBS@
libscerenderer_la_LDFLAGS   = -version-info @SCE_RENDERER_LTVERSION@
libscerenderer_la_SOURCES   = SCERType.c \
                              SCERLight.c \
                              SCERSupport.c \
                              SCERPointSprite.c \
                              SCERMatrix.c \
                              SCERBuffer.c \
                              SCERVertexArray.c \
                              SCERVertexBuffer.c \
                              SCERFeedback.c \
                              SCERShader.c \
                              SCERenderer.c \
                              SCERFramebuffer.c \
                              SCERMaterial.c \
                              SCEROcclusionQuery.c \
                              SCERTexture.c
