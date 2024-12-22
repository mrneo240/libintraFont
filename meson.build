project('sinusoid', 'c',
  version : '0.1',
  default_options : ['c_std=gnu11', 'b_staticpic=false'])

# Compiler arguments
arg_base = ['-w']
arg_code = []
arg_linker = []

# Dependencies
cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : false)

# Options
opt_platform = get_option('platform')

inc_base = []
inc_base += include_directories('include')

src_files = ['src/intraFont.c', 'src/libccc.c']

# Determine Platform
platform_dep = []
platform_inc = []
platform_src = []

if opt_platform == 'glfw'
glfw_dep = dependency('glfw3', fallback : ['glfw3', 'glfw3_dep'])
glad_dep = dependency('glad', fallback : ['glad', 'glad_dep'])
platform_dep += [glfw_dep , glad_dep]

arg_code = ['-DDESKTOP']

elif opt_platform == 'psp'
  arg_code += ['-DPLATFORM_PSP']
  arg_code += ['-DPATH_ASSETS=disc0:/PSP_GAME/USRDIR/', '-DPATH_USERDATA=ms0:/']
  arg_base += ['-DNO_INTRO', '-DHAS_WARNING', '-DDEBUG']
  arg_base += ['-fsingle-precision-constant', '-ffast-math', '-O3', '-g0', '-fexpensive-optimizations', '-fomit-frame-pointer', '-finline-functions']

  arg_linker += [ '-Wl,--end-group', '-lpspgu', '-lpspvfpu', '-lpspctrl', '-lpspge', '-lpspdisplay', '-lm', '-lpspsdk', '-lpsprtc', '-lpspaudio', '-lpsppower', '-Wl,--start-group', '-lc' ]

  pspsdk_dep = declare_dependency(link_args :  ['-g3', '-Wl,-Map=output.map'],
                                include_directories : ['/usr/local/pspdev/psp/sdk/include'] )

  platform_dep += [pspsdk_dep]

elif opt_platform == 'dc'

arg_code += ['-DPLATFORM_DC', '-DNDEBUG']
arg_base += ['-mpretend-cmove','-fsingle-precision-constant', '-ffast-math', '-O2', '-g0',
               '-fomit-frame-pointer', '-finline-functions',  '-mfsrra', '-mfsca']

dc_debug_dep = declare_dependency(link_args :  ['-g3', '-Wl,-Map=output.map'])
libgldc_dep = dependency('gldc', fallback : ['gldc', 'libgldc_dep'])
platform_dep += [libgldc_dep, dc_debug_dep]

else
  error('No platform chosen!')
endif

src_files += platform_src

inc = [ inc_base, platform_inc ]
src = [ src_files ]
dep = [ m_dep, platform_dep ]

arg_c = [ arg_base, arg_code ]

static_lib = static_library('libintrafont',
           c_args: arg_c,
           link_args: arg_linker,
           sources: src,
           include_directories : inc,
           dependencies: dep)
