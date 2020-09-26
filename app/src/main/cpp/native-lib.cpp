#include <jni.h>
#include <string>
#include <android/log.h>
#include "md5.h"

static  char *MD5TITLE = "LIMD5";
static int is_verify = 0;
static char *PACKAGE_NAME = "com.li.md5sign";
static  char *APP_SIGNATURE= "308202e4308201cc020101300d06092a864886f70d010105050030373116301406035504030c0d416e64726f69642044656275673110300e060355040a0c07416e64726f6964310b30090603550406130255533020170d3230303431333035313432335a180f32303530303430363035313432335a30373116301406035504030c0d416e64726f69642044656275673110300e060355040a0c07416e64726f6964310b300906035504061302555330820122300d06092a864886f70d01010105000382010f003082010a0282010100b3734ac8d1ecf7e30be02a75bdaae0fd98b044cb12e007405b815636a1aa83d868551740356ac33643ff9294dbf011569b2776be8e8a318de33a56cf50c9ac93dbabce53f64239dd21908a5d252e37fcd42e57a3a6bad84f5b7eafb268038e3645c3d32c4a985731be3d4ed0a26ecbf46ab2d0b3d0d4910d75a5261f161aa438f2b7f14449a6044ca396620ad9351b0ec9cf560d16c771b3a3214124fd933ddeb6325a906de07ad914c9a084f47594d60e749aceca6e8be1e4e343e75cc62d36bdf7a27690f58103b65934a0d9f83875503e3dcf2c09c8db2ead37818b176d59b0ff5c73cd26af6a4ecf03fe21c38e460ad47bf36d0ae7669a8b38835d2f9e790203010001300d06092a864886f70d01010505000382010100148bf14237f11ac18c4ad0e66989794fb11f1b151c29b6c217b2b3e3d9d615a53b4dc2ea1b94d49cfe0aff7d4904a6c97846fd52972ecebf4100e5bd4999631c9d94540f4889a6122a8478510b907046f541ea0bd6b1d1c6a59583b03c91acbf9b1420630aa615177aa0ed34da508bf16be0893234d72d78570314df5c437f9c57184ce5cb93ee13ab2fa421da4e66215c3bd378aece0917a2f6f487c9453a22662d148aea11bc6f27ecda3489be4c86c738f03fb64200b9e0b11203a6acda227fb6d7dec3290c61400fa529431a6372ac46298f2937c8153c1677e884045d3cd63b31728de92c635807a594ea5e3faf036e66d9c4fb4ff17e30cd0913717500";
using namespace std ;

extern "C"
JNIEXPORT jstring JNICALL
Java_com_li_md5sign_MD5SignUtils_signParams(JNIEnv *env, jclass clazz,jstring params_) {
    // TODO: implement signParams()
    if (is_verify == 0) {
        return env->NewStringUTF("error_signature");
    }
    //1.传过来的String转成char
    const char *params = env->GetStringUTFChars(params_,0);
    __android_log_print(ANDROID_LOG_ERROR,"JNI_TAG","params_的值:%s",params);
    //2.字符串前后做点手脚
    string signature_str(params);
    signature_str.insert(0,MD5TITLE);
    signature_str = signature_str.substr(0,signature_str.length()-2);
    //3 MD5加密
    MD5_CTX *ctx = new MD5_CTX();
    MD5Init(ctx);
    MD5Update(ctx, (unsigned char *) signature_str.c_str(), signature_str.length());
    unsigned char digest[16] = {0};
    MD5Final(digest,ctx);

    char md5_str[32] = {0};
    for (int i = 0; i < 16;i++) {
        sprintf(md5_str,"%s%02x",md5_str,digest[i]);
    }

    env->ReleaseStringUTFChars(params_,params);
    __android_log_print(ANDROID_LOG_ERROR,"JNI_TAG","md5_str的值:%s",md5_str);

    return env->NewStringUTF(md5_str);

}
//packageInfo = getPackageManager().getPackageInfo(getPackageName(), PackageManager.GET_SIGNATURES);
//Signature[] signatures = packageInfo.signatures;
//return signatures[0].toCharsString();
extern "C"
JNIEXPORT void JNICALL
Java_com_li_md5sign_MD5SignUtils_checkSignature(JNIEnv *env, jclass clazz, jobject context) {
    // TODO: implement checkSignature()
    //1 获取包名
    jclass  j_clz = env->GetObjectClass(context);
    jmethodID j_mid = env->GetMethodID(j_clz,"getPackageName","()Ljava/lang/String;");
    jstring j_package_name = static_cast<jstring>(env->CallObjectMethod(context, j_mid));
    //2 对比包名
    const  char *c_package_name = env->GetStringUTFChars(j_package_name,NULL);
    if(strcmp(c_package_name,PACKAGE_NAME)!=0){
        return;
    }
    //3 校验App的签名
   //3.1 获取PackageManager
   j_mid = env->GetMethodID(j_clz,"getPackageManager","()Landroid/content/pm/PackageManager;");
   jobject  j_package_manger = env->CallObjectMethod(context,j_mid);
   //3.2 获取PackageInfo
    j_clz = env->GetObjectClass(j_package_manger);
    j_mid = env->GetMethodID(j_clz,"getPackageInfo","(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
    jobject  j_package_info = env->CallObjectMethod(j_package_manger,j_mid,j_package_name,0x00000040);
   //3.3 获取Signature[]
    j_clz = env->GetObjectClass(j_package_info);
    jfieldID  j_fid = env->GetFieldID(j_clz,"signatures","[Landroid/content/pm/Signature;");
    jobjectArray j_signature = static_cast<jobjectArray>(env->GetObjectField(j_package_info,
                                                                             j_fid));
   //3.4 获取Signature[0]
    jobject signature_first = env->GetObjectArrayElement(j_signature,0);
   // 3.5 调用 signatures[0].toCharsString();
    j_clz = env->GetObjectClass(signature_first);
    j_mid = env->GetMethodID(j_clz,"toCharsString","()Ljava/lang/String;");
    jstring j_signature_str = static_cast<jstring>(env->CallObjectMethod(signature_first, j_mid));
    const char * c_signature_str = env->GetStringUTFChars(j_signature_str,NULL);
   // 4. 比对签名是否一样
    if (strcmp(c_signature_str,APP_SIGNATURE)!=0){
        return;
    }
    __android_log_print(ANDROID_LOG_ERROR,"JNI","校验签名成功 ：%s",c_signature_str);
    is_verify = 1;
}