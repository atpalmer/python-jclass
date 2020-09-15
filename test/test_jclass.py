import jclass


def test_jclass():
    jcls = jclass.load('java/HelloWorld.class')
    assert isinstance(jcls, jclass.JavaClass)


def test_JavaClass_magic_number():
    jcls = jclass.load('java/HelloWorld.class')
    assert jcls.magic_number == 0xCAFEBABE
