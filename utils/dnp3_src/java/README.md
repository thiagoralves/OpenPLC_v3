# Java bindings

The opendnp3 Java bindings are built as two separate pieces: a normal Java JAR file and a native shared library
built from the companion C++ source.

The JNI C++ code uses a code generator (in the 'codegen' folder) to create accessors for manipulating java objects
from C++. The entry point for this program is in 'Generator.scala':

https://github.com/automatak/dnp3/blob/2.0.x/java/codegen/src/main/scala/com/automatak/dnp3/codegen/Generator.scala

The POM imports nicely into IntelliJ. The generator is hardwired to spit out the C++ files to the correct location
when run from the project root (this directory where the POM is located). This is the default for IntelliJ.

I use Intellij to run this program. It should just work when you import the pom into IntelliJ. Maven will also build it.

The program is configured to generate only what's required for the objects needed. The configuration is in "Classes.scala":

https://github.com/automatak/dnp3/blob/2.0.x/java/codegen/src/main/scala/com/automatak/dnp3/codegen/Classes.scala

The generator uses reflection to product the C++ wrappers.

The wrappers all looks like this example:

https://github.com/automatak/dnp3/blob/2.0.x/java/cpp/jni/JNIAnalogOutputInt16.h
https://github.com/automatak/dnp3/blob/2.0.x/java/cpp/jni/JNIAnalogOutputInt16.cpp

Each wrapper is a static class that gets initialized here:

https://github.com/automatak/dnp3/blob/2.0.x/java/cpp/jni/JCache.cpp#L110

This invoked when the library is loaded:

https://github.com/automatak/dnp3/blob/2.0.x/java/cpp/adapters/JNI.cpp#L31

This approach has a few benefits:

1. The hand-written code never has to deal with strings or variable length function signatures.
2. All of the jclass, jmethodid, and jfieldid stuff is optimally cached.
3. Changes to java classes usually result in compile-time errors.
4. Management types like LocalRef<jobject> are returned to ensure there are no resource leaks.
