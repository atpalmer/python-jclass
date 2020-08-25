import jclass


def test_jclass():
    jcls = jclass.load('java/HelloWorld.class')
    assert jcls == False

