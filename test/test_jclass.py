import pytest
import jclass


def test_jclass():
    jcls = jclass.load('java/HelloWorld.class')
    assert isinstance(jcls, jclass.JavaClass)


def test_JavaClass_magic():
    jcls = jclass.load('java/HelloWorld.class')
    assert jcls.magic == 0xCAFEBABE


def test_JavaClass_minor_version():
    jcls = jclass.load('java/HelloWorld.class')
    assert jcls.minor_version == 0


def test_JavaClass_major_version():
    jcls = jclass.load('java/HelloWorld.class')
    assert jcls.major_version == 58


def test_JavaClass_constant_fail():
    jcls = jclass.load('java/HelloWorld.class')
    with pytest.raises(IndexError):
        assert jcls.constant(0) == True


def test_JavaClass_constant_ok():
    jcls = jclass.load('java/HelloWorld.class')
    constant = jcls.constant(1)
    assert constant == None


def test_JavaClass_this_class():
    jcls = jclass.load('java/HelloWorld.class')
    assert jcls.name == 'HelloWorld'


def test_JavaClass_this_class():
    jcls = jclass.load('java/HelloWorld.class')
    assert jcls.superclass_name == 'java/lang/Object'
