package com.testthb.common_base.utils

object GlslDataCenter {
    private val sampleKeyValue = LinkedHashMap<String,Int>()
    private val vertexGlsl = LinkedHashMap<Int,String>()
    private val fragmentGlsl = LinkedHashMap<Int,String>()
    init {


        // ========================  GLSL ==========================
        // 显示键值对
        sampleKeyValue.apply {
            put("Triangle",1)
            put("Texture",2)
            put("TextureMapSample",3)
            put("TextureCubeSample",9)
            put("TestFBOSample",10)
            put("LightSample",11)

            put("Model3DSample",12)
            put("DepthTestSample",13)
            put("BlendSample",14)
            put("SeniorGLSLSample",15)
            put("InstanceSample",16)

            put("AntiAliasSample",17)
            put("ShadowSample",18)
            put("ShadowCubeSample",19)
        }

        vertexGlsl.apply {
            put(1,"glsl/triangle/vetex.glsl")
            put(2,"glsl/texture/vetex.glsl")
            put(9,"glsl/cube/vetex.glsl")
            put(10,"glsl/fbo/vetex.glsl")
            put(11,"glsl/light/vetex.glsl")
            put(12,"glsl/model3d/vetex.glsl")

            put(13,"glsl/depth_test/vetex.glsl")
            put(14,"glsl/blend/vetex.glsl")
            put(15,"glsl/seniorglsl/vetex.glsl")
            put(16,"glsl/instance/vetex.glsl")
            put(17,"glsl/antialias/vetex.glsl")
            put(18,"glsl/shadow/vetex.glsl")

            put(19,"glsl/shadowcube/vetex.glsl")
        }

        fragmentGlsl.apply {
            put(1,"glsl/triangle/fragment.glsl")
            put(2,"glsl/texture/fragment.glsl")
            put(9,"glsl/cube/fragment.glsl")
            put(10,"glsl/fbo/fragment.glsl")
            put(11,"glsl/light/fragment.glsl")
            put(12,"glsl/model3d/fragment.glsl")

            put(13,"glsl/depth_test/fragment.glsl")
            put(14,"glsl/blend/fragment.glsl")
            put(15,"glsl/seniorglsl/fragment.glsl")
            put(16,"glsl/instance/fragment.glsl")
            put(17,"glsl/antialias/fragment.glsl")
            put(18,"glsl/shadow/fragment.glsl")
            put(19,"glsl/shadowcube/fragment.glsl")
        }
        // ==========================  GLSL End ==============================
    }
    fun getSampleKeys():MutableList<String>{
        return ArrayList(sampleKeyValue.keys)
    }

    fun getTypeByName(type:String):Int{
        return sampleKeyValue[type] ?: 1
    }

    fun getVertexStrByType(type:Int):String{
        return vertexGlsl[type] ?: ""
    }

    fun getFragmentStrByType(type:Int):String{
        return fragmentGlsl[type] ?: ""
    }
}