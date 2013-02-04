import platform
import os

env = Environment()

cpus = 0
if platform.system() == 'Windows':
    env['windows'] = True
    cpus = os.environ['NUMBER_OF_PROCESSORS']
else:
    env['windows'] = False
    env['CXX'] = os.environ['CXX']
    env['ENV']['TERM'] = os.environ['TERM']
    cpus = os.sysconf('SC_NPROCESSORS_ONLN')

    if len(env['CXX']) == 0 :
        env['CXX'] = 'g++-4.7'

# max jobs?
if cpus <= 0:
    cpus = 1
else:
    cpus = int(float(cpus) * 1.5)

parallel = int(ARGUMENTS.get('parallel', -1))

if parallel == -1:
    SetOption('num_jobs', cpus)
elif parallel == 0:
    SetOption('num_jobs', 1)
else:
    SetOption('num_jobs', parallel)

# what are we building?
debug = ARGUMENTS.get('debug', 0)
release = ARGUMENTS.get('release', 0)
package = ARGUMENTS.get('package', 0)

if env['windows'] == True:
    package = 0

# if building the package, need to build both
if package != 0:
    env['version'] = raw_input("Enter Version String: ")
    release = 1
    debug = 1

# default to building just debug version if nothing is set
if debug == 0 and release == 0:
    debug = 1

def buildProjects(env):
    tinyxml2 = SConscript('external/tinyxml2/sconscript', exports = 'env')
    env['tinyxml2'] = tinyxml2

    xUnit = SConscript('xUnit++/sconscript', exports = 'env')
    env['xUnit'] = xUnit

    xUnitUtility = SConscript('xUnit++.Utility/sconscript', exports = 'env')
    env['xUnitUtility'] = xUnitUtility

    console = SConscript('xUnit++.console/sconscript', exports = 'env')
    Depends(console, xUnit)

    if ARGUMENTS.get('test', 1) == 1:

        testHelpers = SConscript('Tests/Helpers/sconscript', exports = 'env')
        env['testHelpers'] = testHelpers

        bareTests = SConscript('Tests/BareTests/sconscript', exports = 'env')
        unitTests = SConscript('Tests/UnitTests/sconscript', exports = 'env')
        utilityTests = SConscript('Tests/UtilityTests/sconscript', exports = 'env')

        Depends(bareTests, [xUnit, console])
        Depends(unitTests, [xUnit, console])
        Depends(utilityTests, [xUnit, console])

        AddPostAction(bareTests, Action(str(console[0]) + " " + str(bareTests[0])))
        AddPostAction(unitTests, Action(str(console[0]) + " " + str(unitTests[0]) + " -g"))
        AddPostAction(utilityTests, Action(str(console[0]) + " " + str(utilityTests[0]) + " -g"))

if debug != 0:
    env['debug'] = True

    SConscript('.build/output.sconscript', exports = 'env')
    SConscript('.build/build.sconscript', exports = 'env')

    dbg = SConscript('.build/debug.sconscript', exports = 'env')
    buildProjects(dbg)

if release != 0:
    env['debug'] = False

    SConscript('.build/output.sconscript', exports = 'env')
    SConscript('.build/build.sconscript', exports = 'env')

    rel = SConscript('.build/release.sconscript', exports = 'env')
    buildProjects(rel)

    if package != 0:
        SConscript('.build/package.sconscript', exports = 'env')
