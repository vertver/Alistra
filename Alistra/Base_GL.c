#include "Base.h"
#include "Base_Window.h"
#include "Base_Render.h"

typedef BOOL(WINAPI* wglMakeCurrent_type)(HDC, HGLRC);
typedef BOOL(WINAPI* wglDeleteContext_type)(HGLRC);
typedef PROC(WINAPI* wglGetProcAddress_type)(LPCSTR);
typedef HGLRC(WINAPI* wglCreateContext_type)(HDC);

wglCreateContext_type pwglCreateContext = NULL;
wglMakeCurrent_type	pwglMakeCurrent = NULL;
wglDeleteContext_type pwglDeleteContext = NULL;
wglGetProcAddress_type pwglGetProcAddress = NULL;
PFNGLCULLFACEPROC gl3wCullFace;
PFNGLFRONTFACEPROC gl3wFrontFace;
PFNGLHINTPROC gl3wHint;
PFNGLLINEWIDTHPROC gl3wLineWidth;
PFNGLPOINTSIZEPROC gl3wPointSize;
PFNGLPOLYGONMODEPROC gl3wPolygonMode;
PFNGLSCISSORPROC gl3wScissor;
PFNGLTEXPARAMETERFPROC gl3wTexParameterf;
PFNGLTEXPARAMETERFVPROC gl3wTexParameterfv;
PFNGLTEXPARAMETERIPROC gl3wTexParameteri;
PFNGLTEXPARAMETERIVPROC gl3wTexParameteriv;
PFNGLTEXIMAGE1DPROC gl3wTexImage1D;
PFNGLTEXIMAGE2DPROC gl3wTexImage2D;
PFNGLDRAWBUFFERPROC gl3wDrawBuffer;
PFNGLCLEARPROC gl3wClear;
PFNGLCLEARCOLORPROC gl3wClearColor;
PFNGLCLEARSTENCILPROC gl3wClearStencil;
PFNGLCLEARDEPTHPROC gl3wClearDepth;
PFNGLSTENCILMASKPROC gl3wStencilMask;
PFNGLCOLORMASKPROC gl3wColorMask;
PFNGLDEPTHMASKPROC gl3wDepthMask;
PFNGLDISABLEPROC gl3wDisable;
PFNGLENABLEPROC gl3wEnable;
PFNGLFINISHPROC gl3wFinish;
PFNGLFLUSHPROC gl3wFlush;
PFNGLBLENDFUNCPROC gl3wBlendFunc;
PFNGLLOGICOPPROC gl3wLogicOp;
PFNGLSTENCILFUNCPROC gl3wStencilFunc;
PFNGLSTENCILOPPROC gl3wStencilOp;
PFNGLDEPTHFUNCPROC gl3wDepthFunc;
PFNGLPIXELSTOREFPROC gl3wPixelStoref;
PFNGLPIXELSTOREIPROC gl3wPixelStorei;
PFNGLREADBUFFERPROC gl3wReadBuffer;
PFNGLREADPIXELSPROC gl3wReadPixels;
PFNGLGETBOOLEANVPROC gl3wGetBooleanv;
PFNGLGETDOUBLEVPROC gl3wGetDoublev;
PFNGLGETERRORPROC gl3wGetError;
PFNGLGETFLOATVPROC gl3wGetFloatv;
PFNGLGETINTEGERVPROC gl3wGetIntegerv;
PFNGLGETSTRINGPROC gl3wGetString;
PFNGLGETTEXIMAGEPROC gl3wGetTexImage;
PFNGLGETTEXPARAMETERFVPROC gl3wGetTexParameterfv;
PFNGLGETTEXPARAMETERIVPROC gl3wGetTexParameteriv;
PFNGLGETTEXLEVELPARAMETERFVPROC gl3wGetTexLevelParameterfv;
PFNGLGETTEXLEVELPARAMETERIVPROC gl3wGetTexLevelParameteriv;
PFNGLISENABLEDPROC gl3wIsEnabled;
PFNGLDEPTHRANGEPROC gl3wDepthRange;
PFNGLVIEWPORTPROC gl3wViewport;
PFNGLDRAWARRAYSPROC gl3wDrawArrays;
PFNGLDRAWELEMENTSPROC gl3wDrawElements;
PFNGLGETPOINTERVPROC gl3wGetPointerv;
PFNGLPOLYGONOFFSETPROC gl3wPolygonOffset;
PFNGLCOPYTEXIMAGE1DPROC gl3wCopyTexImage1D;
PFNGLCOPYTEXIMAGE2DPROC gl3wCopyTexImage2D;
PFNGLCOPYTEXSUBIMAGE1DPROC gl3wCopyTexSubImage1D;
PFNGLCOPYTEXSUBIMAGE2DPROC gl3wCopyTexSubImage2D;
PFNGLTEXSUBIMAGE1DPROC gl3wTexSubImage1D;
PFNGLTEXSUBIMAGE2DPROC gl3wTexSubImage2D;
PFNGLBINDTEXTUREPROC gl3wBindTexture;
PFNGLDELETETEXTURESPROC gl3wDeleteTextures;
PFNGLGENTEXTURESPROC gl3wGenTextures;
PFNGLISTEXTUREPROC gl3wIsTexture;
PFNGLBLENDCOLORPROC gl3wBlendColor;
PFNGLBLENDEQUATIONPROC gl3wBlendEquation;
PFNGLDRAWRANGEELEMENTSPROC gl3wDrawRangeElements;
PFNGLTEXIMAGE3DPROC gl3wTexImage3D;
PFNGLTEXSUBIMAGE3DPROC gl3wTexSubImage3D;
PFNGLCOPYTEXSUBIMAGE3DPROC gl3wCopyTexSubImage3D;
PFNGLGENQUERIESPROC gl3wGenQueries;
PFNGLDELETEQUERIESPROC gl3wDeleteQueries;
PFNGLISQUERYPROC gl3wIsQuery;
PFNGLBEGINQUERYPROC gl3wBeginQuery;
PFNGLENDQUERYPROC gl3wEndQuery;
PFNGLGETQUERYIVPROC gl3wGetQueryiv;
PFNGLGETQUERYOBJECTIVPROC gl3wGetQueryObjectiv;
PFNGLGETQUERYOBJECTUIVPROC gl3wGetQueryObjectuiv;
PFNGLBINDBUFFERPROC gl3wBindBuffer;
PFNGLDELETEBUFFERSPROC gl3wDeleteBuffers;
PFNGLGENBUFFERSPROC gl3wGenBuffers;
PFNGLISBUFFERPROC gl3wIsBuffer;
PFNGLBUFFERDATAPROC gl3wBufferData;
PFNGLBUFFERSUBDATAPROC gl3wBufferSubData;
PFNGLGETBUFFERSUBDATAPROC gl3wGetBufferSubData;
PFNGLMAPBUFFERPROC gl3wMapBuffer;
PFNGLUNMAPBUFFERPROC gl3wUnmapBuffer;
PFNGLGETBUFFERPARAMETERIVPROC gl3wGetBufferParameteriv;
PFNGLGETBUFFERPOINTERVPROC gl3wGetBufferPointerv;
PFNGLBLENDEQUATIONSEPARATEPROC gl3wBlendEquationSeparate;
PFNGLDRAWBUFFERSPROC gl3wDrawBuffers;
PFNGLSTENCILOPSEPARATEPROC gl3wStencilOpSeparate;
PFNGLSTENCILFUNCSEPARATEPROC gl3wStencilFuncSeparate;
PFNGLSTENCILMASKSEPARATEPROC gl3wStencilMaskSeparate;
PFNGLATTACHSHADERPROC gl3wAttachShader;
PFNGLBINDATTRIBLOCATIONPROC gl3wBindAttribLocation;
PFNGLCOMPILESHADERPROC gl3wCompileShader;
PFNGLCREATEPROGRAMPROC gl3wCreateProgram;
PFNGLCREATESHADERPROC gl3wCreateShader;
PFNGLDELETEPROGRAMPROC gl3wDeleteProgram;
PFNGLDELETESHADERPROC gl3wDeleteShader;
PFNGLDETACHSHADERPROC gl3wDetachShader;
PFNGLDISABLEVERTEXATTRIBARRAYPROC gl3wDisableVertexAttribArray;
PFNGLENABLEVERTEXATTRIBARRAYPROC gl3wEnableVertexAttribArray;
PFNGLGETACTIVEATTRIBPROC gl3wGetActiveAttrib;
PFNGLGETACTIVEUNIFORMPROC gl3wGetActiveUniform;
PFNGLGETATTACHEDSHADERSPROC gl3wGetAttachedShaders;
PFNGLGETATTRIBLOCATIONPROC gl3wGetAttribLocation;
PFNGLGETPROGRAMIVPROC gl3wGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC gl3wGetProgramInfoLog;
PFNGLGETSHADERIVPROC gl3wGetShaderiv;
PFNGLGETSHADERINFOLOGPROC gl3wGetShaderInfoLog;
PFNGLGETSHADERSOURCEPROC gl3wGetShaderSource;
PFNGLGETUNIFORMLOCATIONPROC gl3wGetUniformLocation;
PFNGLGETUNIFORMFVPROC gl3wGetUniformfv;
PFNGLGETUNIFORMIVPROC gl3wGetUniformiv;
PFNGLGETVERTEXATTRIBDVPROC gl3wGetVertexAttribdv;
PFNGLGETVERTEXATTRIBFVPROC gl3wGetVertexAttribfv;
PFNGLGETVERTEXATTRIBIVPROC gl3wGetVertexAttribiv;
PFNGLGETVERTEXATTRIBPOINTERVPROC gl3wGetVertexAttribPointerv;
PFNGLISPROGRAMPROC gl3wIsProgram;
PFNGLISSHADERPROC gl3wIsShader;
PFNGLLINKPROGRAMPROC gl3wLinkProgram;
PFNGLSHADERSOURCEPROC gl3wShaderSource;
PFNGLUSEPROGRAMPROC gl3wUseProgram;
PFNGLUNIFORM1FPROC gl3wUniform1f;
PFNGLUNIFORM2FPROC gl3wUniform2f;
PFNGLUNIFORM3FPROC gl3wUniform3f;
PFNGLUNIFORM4FPROC gl3wUniform4f;
PFNGLUNIFORM1IPROC gl3wUniform1i;
PFNGLUNIFORM2IPROC gl3wUniform2i;
PFNGLUNIFORM3IPROC gl3wUniform3i;
PFNGLUNIFORM4IPROC gl3wUniform4i;
PFNGLUNIFORM1FVPROC gl3wUniform1fv;
PFNGLUNIFORM2FVPROC gl3wUniform2fv;
PFNGLUNIFORM3FVPROC gl3wUniform3fv;
PFNGLUNIFORM4FVPROC gl3wUniform4fv;
PFNGLUNIFORM1IVPROC gl3wUniform1iv;
PFNGLUNIFORM2IVPROC gl3wUniform2iv;
PFNGLUNIFORM3IVPROC gl3wUniform3iv;
PFNGLUNIFORM4IVPROC gl3wUniform4iv;
PFNGLUNIFORMMATRIX2FVPROC gl3wUniformMatrix2fv;
PFNGLUNIFORMMATRIX3FVPROC gl3wUniformMatrix3fv;
PFNGLUNIFORMMATRIX4FVPROC gl3wUniformMatrix4fv;
PFNGLVALIDATEPROGRAMPROC gl3wValidateProgram;
PFNGLVERTEXATTRIB1DPROC gl3wVertexAttrib1d;
PFNGLVERTEXATTRIB1DVPROC gl3wVertexAttrib1dv;
PFNGLVERTEXATTRIB1FPROC gl3wVertexAttrib1f;
PFNGLVERTEXATTRIB1FVPROC gl3wVertexAttrib1fv;
PFNGLVERTEXATTRIB1SPROC gl3wVertexAttrib1s;
PFNGLVERTEXATTRIB1SVPROC gl3wVertexAttrib1sv;
PFNGLVERTEXATTRIB2DPROC gl3wVertexAttrib2d;
PFNGLVERTEXATTRIB2DVPROC gl3wVertexAttrib2dv;
PFNGLVERTEXATTRIB2FPROC gl3wVertexAttrib2f;
PFNGLVERTEXATTRIB2FVPROC gl3wVertexAttrib2fv;
PFNGLVERTEXATTRIB2SPROC gl3wVertexAttrib2s;
PFNGLVERTEXATTRIB2SVPROC gl3wVertexAttrib2sv;
PFNGLVERTEXATTRIB3DPROC gl3wVertexAttrib3d;
PFNGLVERTEXATTRIB3DVPROC gl3wVertexAttrib3dv;
PFNGLVERTEXATTRIB3FPROC gl3wVertexAttrib3f;
PFNGLVERTEXATTRIB3FVPROC gl3wVertexAttrib3fv;
PFNGLVERTEXATTRIB3SPROC gl3wVertexAttrib3s;
PFNGLVERTEXATTRIB3SVPROC gl3wVertexAttrib3sv;
PFNGLVERTEXATTRIB4NBVPROC gl3wVertexAttrib4Nbv;
PFNGLVERTEXATTRIB4NIVPROC gl3wVertexAttrib4Niv;
PFNGLVERTEXATTRIB4NSVPROC gl3wVertexAttrib4Nsv;
PFNGLVERTEXATTRIB4NUBPROC gl3wVertexAttrib4Nub;
PFNGLVERTEXATTRIB4NUBVPROC gl3wVertexAttrib4Nubv;
PFNGLVERTEXATTRIB4NUIVPROC gl3wVertexAttrib4Nuiv;
PFNGLVERTEXATTRIB4NUSVPROC gl3wVertexAttrib4Nusv;
PFNGLVERTEXATTRIB4BVPROC gl3wVertexAttrib4bv;
PFNGLVERTEXATTRIB4DPROC gl3wVertexAttrib4d;
PFNGLVERTEXATTRIB4DVPROC gl3wVertexAttrib4dv;
PFNGLVERTEXATTRIB4FPROC gl3wVertexAttrib4f;
PFNGLVERTEXATTRIB4FVPROC gl3wVertexAttrib4fv;
PFNGLVERTEXATTRIB4IVPROC gl3wVertexAttrib4iv;
PFNGLVERTEXATTRIB4SPROC gl3wVertexAttrib4s;
PFNGLVERTEXATTRIB4SVPROC gl3wVertexAttrib4sv;
PFNGLVERTEXATTRIB4UBVPROC gl3wVertexAttrib4ubv;
PFNGLVERTEXATTRIB4UIVPROC gl3wVertexAttrib4uiv;
PFNGLVERTEXATTRIB4USVPROC gl3wVertexAttrib4usv;
PFNGLVERTEXATTRIBPOINTERPROC gl3wVertexAttribPointer;
PFNGLUNIFORMMATRIX2X3FVPROC gl3wUniformMatrix2x3fv;
PFNGLUNIFORMMATRIX3X2FVPROC gl3wUniformMatrix3x2fv;
PFNGLUNIFORMMATRIX2X4FVPROC gl3wUniformMatrix2x4fv;
PFNGLUNIFORMMATRIX4X2FVPROC gl3wUniformMatrix4x2fv;
PFNGLUNIFORMMATRIX3X4FVPROC gl3wUniformMatrix3x4fv;
PFNGLUNIFORMMATRIX4X3FVPROC gl3wUniformMatrix4x3fv;
PFNGLMAPBUFFERRANGEPROC gl3wMapBufferRange;
PFNGLFLUSHMAPPEDBUFFERRANGEPROC gl3wFlushMappedBufferRange;
PFNGLBINDVERTEXARRAYPROC gl3wBindVertexArray;
PFNGLDELETEVERTEXARRAYSPROC gl3wDeleteVertexArrays;
PFNGLGENVERTEXARRAYSPROC gl3wGenVertexArrays;
PFNGLISVERTEXARRAYPROC gl3wIsVertexArray;
PFNGLCOPYBUFFERSUBDATAPROC gl3wCopyBufferSubData;
PFNGLRELEASESHADERCOMPILERPROC gl3wReleaseShaderCompiler;
PFNGLSHADERBINARYPROC gl3wShaderBinary;
PFNGLGETSHADERPRECISIONFORMATPROC gl3wGetShaderPrecisionFormat;
PFNGLDEPTHRANGEFPROC gl3wDepthRangef;
PFNGLCLEARDEPTHFPROC gl3wClearDepthf;
PFNGLDISPATCHCOMPUTEPROC gl3wDispatchCompute;
PFNGLDISPATCHCOMPUTEINDIRECTPROC gl3wDispatchComputeIndirect;
PFNGLMULTIDRAWARRAYSINDIRECTPROC gl3wMultiDrawArraysIndirect;
PFNGLMULTIDRAWELEMENTSINDIRECTPROC gl3wMultiDrawElementsIndirect;

DWORD GlobalWidth = 0;
DWORD GlobalHeight = 0;

HMODULE libgl;
HDC hDC = NULL;
HGLRC hGLRC = NULL;

char 
OpenGL_Library()
{
	if (!libgl)
	{
		if ((libgl = GetModuleHandleA("opengl32.dll")) == NULL)
		{
			libgl = LoadLibraryA("opengl32.dll");
			if (!libgl) return false;
		}
	}

	return true;
}

void load_procs()
{
	static boolean isFirst = true;

	if (isFirst)
	{
		gl3wCullFace = (PFNGLCULLFACEPROC)get_proc("glCullFace");
		gl3wFrontFace = (PFNGLFRONTFACEPROC)get_proc("glFrontFace");
		gl3wHint = (PFNGLHINTPROC)get_proc("glHint");
		gl3wLineWidth = (PFNGLLINEWIDTHPROC)get_proc("glLineWidth");
		gl3wPointSize = (PFNGLPOINTSIZEPROC)get_proc("glPointSize");
		gl3wPolygonMode = (PFNGLPOLYGONMODEPROC)get_proc("glPolygonMode");
		gl3wScissor = (PFNGLSCISSORPROC)get_proc("glScissor");
		gl3wTexParameterf = (PFNGLTEXPARAMETERFPROC)get_proc("glTexParameterf");
		gl3wTexParameterfv = (PFNGLTEXPARAMETERFVPROC)get_proc("glTexParameterfv");
		gl3wTexParameteri = (PFNGLTEXPARAMETERIPROC)get_proc("glTexParameteri");
		gl3wTexParameteriv = (PFNGLTEXPARAMETERIVPROC)get_proc("glTexParameteriv");
		gl3wTexImage1D = (PFNGLTEXIMAGE1DPROC)get_proc("glTexImage1D");
		gl3wTexImage2D = (PFNGLTEXIMAGE2DPROC)get_proc("glTexImage2D");
		gl3wDrawBuffer = (PFNGLDRAWBUFFERPROC)get_proc("glDrawBuffer");
		gl3wClear = (PFNGLCLEARPROC)get_proc("glClear");
		gl3wClearColor = (PFNGLCLEARCOLORPROC)get_proc("glClearColor");
		gl3wClearStencil = (PFNGLCLEARSTENCILPROC)get_proc("glClearStencil");
		gl3wClearDepth = (PFNGLCLEARDEPTHPROC)get_proc("glClearDepth");
		gl3wStencilMask = (PFNGLSTENCILMASKPROC)get_proc("glStencilMask");
		gl3wColorMask = (PFNGLCOLORMASKPROC)get_proc("glColorMask");
		gl3wDepthMask = (PFNGLDEPTHMASKPROC)get_proc("glDepthMask");
		gl3wDisable = (PFNGLDISABLEPROC)get_proc("glDisable");
		gl3wEnable = (PFNGLENABLEPROC)get_proc("glEnable");
		gl3wFinish = (PFNGLFINISHPROC)get_proc("glFinish");
		gl3wFlush = (PFNGLFLUSHPROC)get_proc("glFlush");
		gl3wBlendFunc = (PFNGLBLENDFUNCPROC)get_proc("glBlendFunc");
		gl3wLogicOp = (PFNGLLOGICOPPROC)get_proc("glLogicOp");
		gl3wStencilFunc = (PFNGLSTENCILFUNCPROC)get_proc("glStencilFunc");
		gl3wStencilOp = (PFNGLSTENCILOPPROC)get_proc("glStencilOp");
		gl3wDepthFunc = (PFNGLDEPTHFUNCPROC)get_proc("glDepthFunc");
		gl3wPixelStoref = (PFNGLPIXELSTOREFPROC)get_proc("glPixelStoref");
		gl3wPixelStorei = (PFNGLPIXELSTOREIPROC)get_proc("glPixelStorei");
		gl3wReadBuffer = (PFNGLREADBUFFERPROC)get_proc("glReadBuffer");
		gl3wReadPixels = (PFNGLREADPIXELSPROC)get_proc("glReadPixels");
		gl3wGetBooleanv = (PFNGLGETBOOLEANVPROC)get_proc("glGetBooleanv");
		gl3wGetDoublev = (PFNGLGETDOUBLEVPROC)get_proc("glGetDoublev");
		gl3wGetError = (PFNGLGETERRORPROC)get_proc("glGetError");
		gl3wGetFloatv = (PFNGLGETFLOATVPROC)get_proc("glGetFloatv");
		gl3wGetIntegerv = (PFNGLGETINTEGERVPROC)get_proc("glGetIntegerv");
		gl3wGetString = (PFNGLGETSTRINGPROC)get_proc("glGetString");
		gl3wGetTexImage = (PFNGLGETTEXIMAGEPROC)get_proc("glGetTexImage");
		gl3wGetTexParameterfv = (PFNGLGETTEXPARAMETERFVPROC)get_proc("glGetTexParameterfv");
		gl3wGetTexParameteriv = (PFNGLGETTEXPARAMETERIVPROC)get_proc("glGetTexParameteriv");
		gl3wGetTexLevelParameterfv = (PFNGLGETTEXLEVELPARAMETERFVPROC)get_proc("glGetTexLevelParameterfv");
		gl3wGetTexLevelParameteriv = (PFNGLGETTEXLEVELPARAMETERIVPROC)get_proc("glGetTexLevelParameteriv");
		gl3wIsEnabled = (PFNGLISENABLEDPROC)get_proc("glIsEnabled");
		gl3wDepthRange = (PFNGLDEPTHRANGEPROC)get_proc("glDepthRange");
		gl3wViewport = (PFNGLVIEWPORTPROC)get_proc("glViewport");
		gl3wDrawArrays = (PFNGLDRAWARRAYSPROC)get_proc("glDrawArrays");
		gl3wDrawElements = (PFNGLDRAWELEMENTSPROC)get_proc("glDrawElements");
		gl3wGetPointerv = (PFNGLGETPOINTERVPROC)get_proc("glGetPointerv");
		gl3wPolygonOffset = (PFNGLPOLYGONOFFSETPROC)get_proc("glPolygonOffset");
		gl3wCopyTexImage1D = (PFNGLCOPYTEXIMAGE1DPROC)get_proc("glCopyTexImage1D");
		gl3wCopyTexImage2D = (PFNGLCOPYTEXIMAGE2DPROC)get_proc("glCopyTexImage2D");
		gl3wCopyTexSubImage1D = (PFNGLCOPYTEXSUBIMAGE1DPROC)get_proc("glCopyTexSubImage1D");
		gl3wCopyTexSubImage2D = (PFNGLCOPYTEXSUBIMAGE2DPROC)get_proc("glCopyTexSubImage2D");
		gl3wTexSubImage1D = (PFNGLTEXSUBIMAGE1DPROC)get_proc("glTexSubImage1D");
		gl3wTexSubImage2D = (PFNGLTEXSUBIMAGE2DPROC)get_proc("glTexSubImage2D");
		gl3wBindTexture = (PFNGLBINDTEXTUREPROC)get_proc("glBindTexture");
		gl3wDeleteTextures = (PFNGLDELETETEXTURESPROC)get_proc("glDeleteTextures");
		gl3wGenTextures = (PFNGLGENTEXTURESPROC)get_proc("glGenTextures");
		gl3wIsTexture = (PFNGLISTEXTUREPROC)get_proc("glIsTexture");
		gl3wBlendColor = (PFNGLBLENDCOLORPROC)get_proc("glBlendColor");
		gl3wBlendEquation = (PFNGLBLENDEQUATIONPROC)get_proc("glBlendEquation");
		gl3wDrawRangeElements = (PFNGLDRAWRANGEELEMENTSPROC)get_proc("glDrawRangeElements");
		gl3wTexImage3D = (PFNGLTEXIMAGE3DPROC)get_proc("glTexImage3D");
		gl3wTexSubImage3D = (PFNGLTEXSUBIMAGE3DPROC)get_proc("glTexSubImage3D");
		gl3wCopyTexSubImage3D = (PFNGLCOPYTEXSUBIMAGE3DPROC)get_proc("glCopyTexSubImage3D");
		gl3wGenQueries = (PFNGLGENQUERIESPROC)get_proc("glGenQueries");
		gl3wDeleteQueries = (PFNGLDELETEQUERIESPROC)get_proc("glDeleteQueries");
		gl3wIsQuery = (PFNGLISQUERYPROC)get_proc("glIsQuery");
		gl3wBeginQuery = (PFNGLBEGINQUERYPROC)get_proc("glBeginQuery");
		gl3wEndQuery = (PFNGLENDQUERYPROC)get_proc("glEndQuery");
		gl3wGetQueryiv = (PFNGLGETQUERYIVPROC)get_proc("glGetQueryiv");
		gl3wGetQueryObjectiv = (PFNGLGETQUERYOBJECTIVPROC)get_proc("glGetQueryObjectiv");
		gl3wGetQueryObjectuiv = (PFNGLGETQUERYOBJECTUIVPROC)get_proc("glGetQueryObjectuiv");
		gl3wBindBuffer = (PFNGLBINDBUFFERPROC)get_proc("glBindBuffer");
		gl3wDeleteBuffers = (PFNGLDELETEBUFFERSPROC)get_proc("glDeleteBuffers");
		gl3wGenBuffers = (PFNGLGENBUFFERSPROC)get_proc("glGenBuffers");
		gl3wIsBuffer = (PFNGLISBUFFERPROC)get_proc("glIsBuffer");
		gl3wBufferData = (PFNGLBUFFERDATAPROC)get_proc("glBufferData");
		gl3wBufferSubData = (PFNGLBUFFERSUBDATAPROC)get_proc("glBufferSubData");
		gl3wGetBufferSubData = (PFNGLGETBUFFERSUBDATAPROC)get_proc("glGetBufferSubData");
		gl3wMapBuffer = (PFNGLMAPBUFFERPROC)get_proc("glMapBuffer");
		gl3wUnmapBuffer = (PFNGLUNMAPBUFFERPROC)get_proc("glUnmapBuffer");
		gl3wGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC)get_proc("glGetBufferParameteriv");
		gl3wGetBufferPointerv = (PFNGLGETBUFFERPOINTERVPROC)get_proc("glGetBufferPointerv");
		gl3wBlendEquationSeparate = (PFNGLBLENDEQUATIONSEPARATEPROC)get_proc("glBlendEquationSeparate");
		gl3wDrawBuffers = (PFNGLDRAWBUFFERSPROC)get_proc("glDrawBuffers");
		gl3wStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC)get_proc("glStencilOpSeparate");
		gl3wStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC)get_proc("glStencilFuncSeparate");
		gl3wStencilMaskSeparate = (PFNGLSTENCILMASKSEPARATEPROC)get_proc("glStencilMaskSeparate");
		gl3wAttachShader = (PFNGLATTACHSHADERPROC)get_proc("glAttachShader");
		gl3wBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)get_proc("glBindAttribLocation");
		gl3wCompileShader = (PFNGLCOMPILESHADERPROC)get_proc("glCompileShader");
		gl3wCreateProgram = (PFNGLCREATEPROGRAMPROC)get_proc("glCreateProgram");
		gl3wCreateShader = (PFNGLCREATESHADERPROC)get_proc("glCreateShader");
		gl3wDeleteProgram = (PFNGLDELETEPROGRAMPROC)get_proc("glDeleteProgram");
		gl3wDeleteShader = (PFNGLDELETESHADERPROC)get_proc("glDeleteShader");
		gl3wDetachShader = (PFNGLDETACHSHADERPROC)get_proc("glDetachShader");
		gl3wDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)get_proc("glDisableVertexAttribArray");
		gl3wEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)get_proc("glEnableVertexAttribArray");
		gl3wGetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC)get_proc("glGetActiveAttrib");
		gl3wGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC)get_proc("glGetActiveUniform");
		gl3wGetAttachedShaders = (PFNGLGETATTACHEDSHADERSPROC)get_proc("glGetAttachedShaders");
		gl3wGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)get_proc("glGetAttribLocation");
		gl3wGetProgramiv = (PFNGLGETPROGRAMIVPROC)get_proc("glGetProgramiv");
		gl3wGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)get_proc("glGetProgramInfoLog");
		gl3wGetShaderiv = (PFNGLGETSHADERIVPROC)get_proc("glGetShaderiv");
		gl3wGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)get_proc("glGetShaderInfoLog");
		gl3wGetShaderSource = (PFNGLGETSHADERSOURCEPROC)get_proc("glGetShaderSource");
		gl3wGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)get_proc("glGetUniformLocation");
		gl3wGetUniformfv = (PFNGLGETUNIFORMFVPROC)get_proc("glGetUniformfv");
		gl3wGetUniformiv = (PFNGLGETUNIFORMIVPROC)get_proc("glGetUniformiv");
		gl3wGetVertexAttribdv = (PFNGLGETVERTEXATTRIBDVPROC)get_proc("glGetVertexAttribdv");
		gl3wGetVertexAttribfv = (PFNGLGETVERTEXATTRIBFVPROC)get_proc("glGetVertexAttribfv");
		gl3wGetVertexAttribiv = (PFNGLGETVERTEXATTRIBIVPROC)get_proc("glGetVertexAttribiv");
		gl3wGetVertexAttribPointerv = (PFNGLGETVERTEXATTRIBPOINTERVPROC)get_proc("glGetVertexAttribPointerv");
		gl3wIsProgram = (PFNGLISPROGRAMPROC)get_proc("glIsProgram");
		gl3wIsShader = (PFNGLISSHADERPROC)get_proc("glIsShader");
		gl3wLinkProgram = (PFNGLLINKPROGRAMPROC)get_proc("glLinkProgram");
		gl3wShaderSource = (PFNGLSHADERSOURCEPROC)get_proc("glShaderSource");
		gl3wUseProgram = (PFNGLUSEPROGRAMPROC)get_proc("glUseProgram");
		gl3wUniform1f = (PFNGLUNIFORM1FPROC)get_proc("glUniform1f");
		gl3wUniform2f = (PFNGLUNIFORM2FPROC)get_proc("glUniform2f");
		gl3wUniform3f = (PFNGLUNIFORM3FPROC)get_proc("glUniform3f");
		gl3wUniform4f = (PFNGLUNIFORM4FPROC)get_proc("glUniform4f");
		gl3wUniform1i = (PFNGLUNIFORM1IPROC)get_proc("glUniform1i");
		gl3wUniform2i = (PFNGLUNIFORM2IPROC)get_proc("glUniform2i");
		gl3wUniform3i = (PFNGLUNIFORM3IPROC)get_proc("glUniform3i");
		gl3wUniform4i = (PFNGLUNIFORM4IPROC)get_proc("glUniform4i");
		gl3wUniform1fv = (PFNGLUNIFORM1FVPROC)get_proc("glUniform1fv");
		gl3wUniform2fv = (PFNGLUNIFORM2FVPROC)get_proc("glUniform2fv");
		gl3wUniform3fv = (PFNGLUNIFORM3FVPROC)get_proc("glUniform3fv");
		gl3wUniform4fv = (PFNGLUNIFORM4FVPROC)get_proc("glUniform4fv");
		gl3wUniform1iv = (PFNGLUNIFORM1IVPROC)get_proc("glUniform1iv");
		gl3wUniform2iv = (PFNGLUNIFORM2IVPROC)get_proc("glUniform2iv");
		gl3wUniform3iv = (PFNGLUNIFORM3IVPROC)get_proc("glUniform3iv");
		gl3wUniform4iv = (PFNGLUNIFORM4IVPROC)get_proc("glUniform4iv");
		gl3wUniformMatrix2fv = (PFNGLUNIFORMMATRIX2FVPROC)get_proc("glUniformMatrix2fv");
		gl3wUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC)get_proc("glUniformMatrix3fv");
		gl3wUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)get_proc("glUniformMatrix4fv");
		gl3wValidateProgram = (PFNGLVALIDATEPROGRAMPROC)get_proc("glValidateProgram");
		gl3wVertexAttrib1d = (PFNGLVERTEXATTRIB1DPROC)get_proc("glVertexAttrib1d");
		gl3wVertexAttrib1dv = (PFNGLVERTEXATTRIB1DVPROC)get_proc("glVertexAttrib1dv");
		gl3wVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC)get_proc("glVertexAttrib1f");
		gl3wVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC)get_proc("glVertexAttrib1fv");
		gl3wVertexAttrib1s = (PFNGLVERTEXATTRIB1SPROC)get_proc("glVertexAttrib1s");
		gl3wVertexAttrib1sv = (PFNGLVERTEXATTRIB1SVPROC)get_proc("glVertexAttrib1sv");
		gl3wVertexAttrib2d = (PFNGLVERTEXATTRIB2DPROC)get_proc("glVertexAttrib2d");
		gl3wVertexAttrib2dv = (PFNGLVERTEXATTRIB2DVPROC)get_proc("glVertexAttrib2dv");
		gl3wVertexAttrib2f = (PFNGLVERTEXATTRIB2FPROC)get_proc("glVertexAttrib2f");
		gl3wVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC)get_proc("glVertexAttrib2fv");
		gl3wVertexAttrib2s = (PFNGLVERTEXATTRIB2SPROC)get_proc("glVertexAttrib2s");
		gl3wVertexAttrib2sv = (PFNGLVERTEXATTRIB2SVPROC)get_proc("glVertexAttrib2sv");
		gl3wVertexAttrib3d = (PFNGLVERTEXATTRIB3DPROC)get_proc("glVertexAttrib3d");
		gl3wVertexAttrib3dv = (PFNGLVERTEXATTRIB3DVPROC)get_proc("glVertexAttrib3dv");
		gl3wVertexAttrib3f = (PFNGLVERTEXATTRIB3FPROC)get_proc("glVertexAttrib3f");
		gl3wVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC)get_proc("glVertexAttrib3fv");
		gl3wVertexAttrib3s = (PFNGLVERTEXATTRIB3SPROC)get_proc("glVertexAttrib3s");
		gl3wVertexAttrib3sv = (PFNGLVERTEXATTRIB3SVPROC)get_proc("glVertexAttrib3sv");
		gl3wVertexAttrib4Nbv = (PFNGLVERTEXATTRIB4NBVPROC)get_proc("glVertexAttrib4Nbv");
		gl3wVertexAttrib4Niv = (PFNGLVERTEXATTRIB4NIVPROC)get_proc("glVertexAttrib4Niv");
		gl3wVertexAttrib4Nsv = (PFNGLVERTEXATTRIB4NSVPROC)get_proc("glVertexAttrib4Nsv");
		gl3wVertexAttrib4Nub = (PFNGLVERTEXATTRIB4NUBPROC)get_proc("glVertexAttrib4Nub");
		gl3wVertexAttrib4Nubv = (PFNGLVERTEXATTRIB4NUBVPROC)get_proc("glVertexAttrib4Nubv");
		gl3wVertexAttrib4Nuiv = (PFNGLVERTEXATTRIB4NUIVPROC)get_proc("glVertexAttrib4Nuiv");
		gl3wVertexAttrib4Nusv = (PFNGLVERTEXATTRIB4NUSVPROC)get_proc("glVertexAttrib4Nusv");
		gl3wVertexAttrib4bv = (PFNGLVERTEXATTRIB4BVPROC)get_proc("glVertexAttrib4bv");
		gl3wVertexAttrib4d = (PFNGLVERTEXATTRIB4DPROC)get_proc("glVertexAttrib4d");
		gl3wVertexAttrib4dv = (PFNGLVERTEXATTRIB4DVPROC)get_proc("glVertexAttrib4dv");
		gl3wVertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC)get_proc("glVertexAttrib4f");
		gl3wVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC)get_proc("glVertexAttrib4fv");
		gl3wVertexAttrib4iv = (PFNGLVERTEXATTRIB4IVPROC)get_proc("glVertexAttrib4iv");
		gl3wVertexAttrib4s = (PFNGLVERTEXATTRIB4SPROC)get_proc("glVertexAttrib4s");
		gl3wVertexAttrib4sv = (PFNGLVERTEXATTRIB4SVPROC)get_proc("glVertexAttrib4sv");
		gl3wVertexAttrib4ubv = (PFNGLVERTEXATTRIB4UBVPROC)get_proc("glVertexAttrib4ubv");
		gl3wVertexAttrib4uiv = (PFNGLVERTEXATTRIB4UIVPROC)get_proc("glVertexAttrib4uiv");
		gl3wVertexAttrib4usv = (PFNGLVERTEXATTRIB4USVPROC)get_proc("glVertexAttrib4usv");
		gl3wVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)get_proc("glVertexAttribPointer");
		gl3wUniformMatrix2x3fv = (PFNGLUNIFORMMATRIX2X3FVPROC)get_proc("glUniformMatrix2x3fv");
		gl3wUniformMatrix3x2fv = (PFNGLUNIFORMMATRIX3X2FVPROC)get_proc("glUniformMatrix3x2fv");
		gl3wUniformMatrix2x4fv = (PFNGLUNIFORMMATRIX2X4FVPROC)get_proc("glUniformMatrix2x4fv");
		gl3wUniformMatrix4x2fv = (PFNGLUNIFORMMATRIX4X2FVPROC)get_proc("glUniformMatrix4x2fv");
		gl3wUniformMatrix3x4fv = (PFNGLUNIFORMMATRIX3X4FVPROC)get_proc("glUniformMatrix3x4fv");
		gl3wUniformMatrix4x3fv = (PFNGLUNIFORMMATRIX4X3FVPROC)get_proc("glUniformMatrix4x3fv");
		gl3wMapBufferRange = (PFNGLMAPBUFFERRANGEPROC)get_proc("glMapBufferRange");
		gl3wFlushMappedBufferRange = (PFNGLFLUSHMAPPEDBUFFERRANGEPROC)get_proc("glFlushMappedBufferRange");
		gl3wBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)get_proc("glBindVertexArray");
		gl3wDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)get_proc("glDeleteVertexArrays");
		gl3wGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)get_proc("glGenVertexArrays");
		gl3wIsVertexArray = (PFNGLISVERTEXARRAYPROC)get_proc("glIsVertexArray");
		gl3wCopyBufferSubData = (PFNGLCOPYBUFFERSUBDATAPROC)get_proc("glCopyBufferSubData");
		gl3wReleaseShaderCompiler = (PFNGLRELEASESHADERCOMPILERPROC)get_proc("glReleaseShaderCompiler");
		gl3wShaderBinary = (PFNGLSHADERBINARYPROC)get_proc("glShaderBinary");
		gl3wGetShaderPrecisionFormat = (PFNGLGETSHADERPRECISIONFORMATPROC)get_proc("glGetShaderPrecisionFormat");
		gl3wDepthRangef = (PFNGLDEPTHRANGEFPROC)get_proc("glDepthRangef");
		gl3wClearDepthf = (PFNGLCLEARDEPTHFPROC)get_proc("glClearDepthf");
		gl3wDispatchCompute = (PFNGLDISPATCHCOMPUTEPROC)get_proc("glDispatchCompute");
		gl3wDispatchComputeIndirect = (PFNGLDISPATCHCOMPUTEINDIRECTPROC)get_proc("glDispatchComputeIndirect");
		gl3wMultiDrawArraysIndirect = (PFNGLMULTIDRAWARRAYSINDIRECTPROC)get_proc("glMultiDrawArraysIndirect");
		gl3wMultiDrawElementsIndirect = (PFNGLMULTIDRAWELEMENTSINDIRECTPROC)get_proc("glMultiDrawElementsIndirect");
		
		isFirst = false;
	}
}

void *get_proc(const char *proc)
{
	void *res = NULL;

	if (!pwglGetProcAddress) {
		pwglGetProcAddress = (wglGetProcAddress_type)GetProcAddress(libgl, "wglGetProcAddress");
	}

	if (pwglGetProcAddress) {
		res = pwglGetProcAddress(proc);
	}

	if (!res)
		res = GetProcAddress(libgl, proc);
	return res;
}

char 
InitRender(
	char IsImgui
)
{
	hDC = GetDC(GetMainWindowHandle());

	if (!pwglCreateContext) 
	{
		pwglCreateContext = (wglDeleteContext_type)get_proc("wglCreateContext");
		if (!pwglCreateContext) return 0;
	}

	if (!pwglDeleteContext) 
	{
		pwglDeleteContext = (wglDeleteContext_type)get_proc("wglDeleteContext");
		if (!pwglDeleteContext) return 0;
	}

	if (!pwglMakeCurrent)
	{
		pwglMakeCurrent = (wglMakeCurrent_type)get_proc("wglMakeCurrent");
		if (!pwglMakeCurrent) return 0;
	}

	hGLRC = pwglCreateContext(hDC);
	pwglMakeCurrent(hDC, hGLRC);

	return 0;
}

void 
DestroyRender()
{
	pwglMakeCurrent(NULL, NULL);
	pwglDeleteContext(hGLRC);
	hGLRC = NULL;

	ReleaseDC(GetMainWindowHandle(), hDC);

	if (libgl) FreeLibrary(libgl);
}

LPDWORD
GetMainWidth()
{
	return &GlobalWidth;
}


LPDWORD
GetMainHeight()
{
	return &GlobalHeight;
}

boolean
ResizeRender(
	int Width,
	int Height
)
{
	if (!Width || !Height) return false;

	glViewport(0, 0, Width, Height);
	glClear(0x00004000);
	glClearColor(0.f, 0.f, 0.f, 0.f);

	GlobalWidth = Width;
	GlobalHeight = Height;

	return true;
}

boolean
RenderDraw()
{
	return true;
}