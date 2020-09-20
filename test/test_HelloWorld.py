import pytest
import jclass


def test_JavaClass_magic():
    jcls = jclass.load('java/HelloWorld/HelloWorld.class')
    assert jcls.magic == 0xCAFEBABE
    assert jcls.minor_version == 0
    assert jcls.major_version == 58
    assert jcls.name == 'HelloWorld'
    assert jcls.superclass_name == 'java/lang/Object'
    assert jcls.access_set == {'public', 'super'}
    assert jcls.is_public is True
    assert jcls.is_final is False
    assert jcls.is_super is True
    assert jcls.is_interface is False
    assert jcls.is_abstract is False
    assert jcls.is_synthetic is False
    assert jcls.is_annotation is False
    assert jcls.is_enum is False
    assert jcls.fields() == [
        ({'final'}, 'Ljava/lang/String;', '_message', {
            'ConstantValue':
                b'\x00\x0f'
        }),
    ]
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
    assert jcls.attributes() == {
            'SourceFile':
                b'\x00\x1f'
    }

