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


def test_JavaClass_name():
    jcls = jclass.load('java/HelloWorld.class')
    assert jcls.name == 'HelloWorld'


def test_JavaClass_superclass_name():
    jcls = jclass.load('java/HelloWorld.class')
    assert jcls.superclass_name == 'java/lang/Object'


def test_JavaClass_access_set():
    jcls = jclass.load('java/HelloWorld.class')
    assert jcls.access_set == {'public', 'super'}


def test_JavaClass_is_public():
    jcls = jclass.load('java/HelloWorld.class')
    assert jcls.is_public is True


def test_JavaClass_is_final():
    jcls = jclass.load('java/HelloWorld.class')
    assert jcls.is_final is False


def test_JavaClass_is_super():
    jcls = jclass.load('java/HelloWorld.class')
    assert jcls.is_super is True


def test_JavaClass_is_interface():
    jcls = jclass.load('java/HelloWorld.class')
    assert jcls.is_interface is False


def test_JavaClass_is_abstract():
    jcls = jclass.load('java/HelloWorld.class')
    assert jcls.is_abstract is False


def test_JavaClass_is_synthetic():
    jcls = jclass.load('java/HelloWorld.class')
    assert jcls.is_synthetic is False


def test_JavaClass_is_annotation():
    jcls = jclass.load('java/HelloWorld.class')
    assert jcls.is_annotation is False


def test_JavaClass_is_enum():
    jcls = jclass.load('java/HelloWorld.class')
    assert jcls.is_enum is False


def test_JavaClass_fields():
    jcls = jclass.load('java/HelloWorld.class')
    assert jcls.fields() == [({'public'}, 'Ljava/lang/String;', '_message')]


def test_JavaClass_methods():
    jcls = jclass.load('java/HelloWorld.class')
    assert jcls.methods() == [
        ({'public'}, '()V', '<init>'),
        ({'public'}, '([Ljava/lang/String;)V', 'main'),
    ]


def test_JavaClass_attributes():
    jcls = jclass.load('java/HelloWorld.class')
    assert jcls.attributes() == {'SourceFile': b'\x00\x1f'}
