# Modifies the environment to add library static target and includes
# mbed must already be part of the environment, this depends on mbed
Import('env')

env.Append(CPPPATH=[
  Dir('drivers').srcnode(),
  Dir('hal/api').srcnode(),
  Dir('utils').srcnode(),
])

env.Prepend(LIBS=env.StaticLibrary('calsol-fw-libs', [
  Glob('drivers/*.cpp'),
  Glob('utils/*.cpp'),
]))

if 'TARGET_LPC15XX' in env['CPPDEFINES']:
  env.Prepend(LIBS=env.StaticLibrary('calsol-fw-libs-lpc15xx', [
    Glob('hal/TARGET_NXP/TARGET_LPC15XX/*.cpp')
  ]))
