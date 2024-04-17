#include <jni.h>

JNIEXPORT void JNICALL Java_com_ibm_pairhmm_PowerPairHmm_initNative
(JNIEnv* env, jclass cls, jclass readDataHolder, jclass haplotypeDataHolder,
 jboolean use_double, jint max_threads)
{
  return;
}

JNIEXPORT void JNICALL Java_com_ibm_pairhmm_PowerPairHmm_computeLikelihoodsNative
(JNIEnv* env, jobject obj,
 jobjectArray readDataArray, jobjectArray haplotypeDataArray, jdoubleArray likelihoodArray)
{
  return;
}

JNIEXPORT void JNICALL Java_com_ibm_pairhmm_PowerPairHmm_doneNative
(JNIEnv* env, jobject obj)
{
  return;
}
