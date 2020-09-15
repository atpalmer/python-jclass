import jclass


def test_jclass():
    jcls = jclass.load('java/HelloWorld.class')
    assert isinstance(jcls, jclass.JavaClass)


def test_JavaClass_magic_number():
    jcls = jclass.load('java/HelloWorld.class')
    assert jcls.magic_number == 0xCAFEBABE


def test_JavaClass_minor_version():
    jcls = jclass.load('java/HelloWorld.class')
    assert jcls.minor_version == 0


def test_JavaClass_major_version():
    jcls = jclass.load('java/HelloWorld.class')
    assert jcls.major_version == 58
