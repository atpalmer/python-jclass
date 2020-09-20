import pytest
import jclass


def test_jclass():
    jcls = jclass.load('java/HelloWorld/HelloWorld.class')
    assert isinstance(jcls, jclass.JavaClass)


def test_jclass_badfile():
    with pytest.raises(ValueError):
        jcls = jclass.load('java/Fail/DeadBeef.class')


def test_jclass_corrupt_pool():
    with pytest.raises(ValueError):
        jcls = jclass.load('java/Fail/CorruptPool.class')


def test_jclass_bad_path():
    with pytest.raises(OSError):
        jcls = jclass.load('java/Fail/bad-path')


def test_jclass_BadVersion():
    with pytest.raises(ValueError):
        jcls = jclass.load('java/Fail/BadVersion.class')


def test_JavaClass_magic():
    jcls = jclass.load('java/HelloWorld/HelloWorld.class')
    assert jcls.magic == 0xCAFEBABE


def test_JavaClass_minor_version():
    jcls = jclass.load('java/HelloWorld/HelloWorld.class')
    assert jcls.minor_version == 0


def test_JavaClass_major_version():
    jcls = jclass.load('java/HelloWorld/HelloWorld.class')
    assert jcls.major_version == 58


def test_JavaClass_name():
    jcls = jclass.load('java/HelloWorld/HelloWorld.class')
    assert jcls.name == 'HelloWorld'


def test_JavaClass_superclass_name():
    jcls = jclass.load('java/HelloWorld/HelloWorld.class')
    assert jcls.superclass_name == 'java/lang/Object'


def test_JavaClass_access_set():
    jcls = jclass.load('java/HelloWorld/HelloWorld.class')
    assert jcls.access_set == {'public', 'super'}


def test_JavaClass_is_public():
    jcls = jclass.load('java/HelloWorld/HelloWorld.class')
    assert jcls.is_public is True


def test_JavaClass_is_final():
    jcls = jclass.load('java/HelloWorld/HelloWorld.class')
    assert jcls.is_final is False


def test_JavaClass_is_super():
    jcls = jclass.load('java/HelloWorld/HelloWorld.class')
    assert jcls.is_super is True


def test_JavaClass_is_interface():
    jcls = jclass.load('java/HelloWorld/HelloWorld.class')
    assert jcls.is_interface is False


def test_JavaClass_is_abstract():
    jcls = jclass.load('java/HelloWorld/HelloWorld.class')
    assert jcls.is_abstract is False


def test_JavaClass_is_synthetic():
    jcls = jclass.load('java/HelloWorld/HelloWorld.class')
    assert jcls.is_synthetic is False


def test_JavaClass_is_annotation():
    jcls = jclass.load('java/HelloWorld/HelloWorld.class')
    assert jcls.is_annotation is False


def test_JavaClass_is_enum():
    jcls = jclass.load('java/HelloWorld/HelloWorld.class')
    assert jcls.is_enum is False


def test_JavaClass_fields():
    jcls = jclass.load('java/HelloWorld/HelloWorld.class')
    assert jcls.fields() == [
        ({'final'}, 'Ljava/lang/String;', '_message', {'ConstantValue': b'\x00\x0f'}),
    ]


def test_JavaClass_methods():
    jcls = jclass.load('java/HelloWorld/HelloWorld.class')
    assert jcls.methods() == [
        ({'public'}, '()V', '<init>', {
            'Code':
                b'\x00\x01\x00\x01\x00\x00\x00\x05*\xb7\x00\x01\xb1'
                b'\x00\x00\x00\x01\x00\x1b\x00\x00\x00\x06\x00\x01'
                b'\x00\x00\x00\x03'
        }),
        ({'public'}, '([Ljava/lang/String;)V', 'main', {
            'Code':
                b'\x00\x02\x00\x01\x00\x00\x00\t\xb2\x00\x07\x12\x0f'
                b'\xb6\x00\x11\xb1\x00\x00\x00\x01\x00\x1b\x00\x00\x00'
                b'\n\x00\x02\x00\x00\x00\x07\x00\x08\x00\x08'
        }),
    ]


def test_JavaClass_attributes():
    jcls = jclass.load('java/HelloWorld/HelloWorld.class')
    assert jcls.attributes() == {'SourceFile': b'\x00\x1f'}
