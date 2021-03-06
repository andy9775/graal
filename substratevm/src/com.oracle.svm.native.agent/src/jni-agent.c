/*
 * Copyright (c) 2017, 2019, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Oracle designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Oracle in the LICENSE file that accompanied this code.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "trace-agent.h"
#include "util.h"

#include <stdlib.h>

static void jni_trace_call(JNIEnv *env, const char *function, jclass clazz, const char *result, ...) {
  jclass caller_class = get_caller_class(0);
  va_list ap;
  va_start(ap, result);
  trace_append_v(env, "jni", clazz, caller_class, function, result, ap);
  va_end(ap);
}

static jclass JNICALL DefineClass(JNIEnv *env, const char *name, jobject loader, const jbyte *buf, jsize bufLen) {
  jclass result = jnifun->DefineClass(env, name, loader, buf, bufLen);
  jni_trace_call(env, "DefineClass", NULL, nn_bool(result), nn_str(name), NULL);
  return result;
}

static jclass JNICALL FindClass(JNIEnv *env, const char *name) {
  jclass result = jnifun->FindClass(env, name);
  jni_trace_call(env, "FindClass", NULL, nn_bool(result), nn_str(name), NULL);
  return result;
}

static jmethodID JNICALL GetMethodID(JNIEnv *env, jclass clazz, const char *name, const char *sig) {
  jmethodID result = jnifun->GetMethodID(env, clazz, name, sig);
  jni_trace_call(env, "GetMethodID", nn_class(clazz), nn_bool(result), nn_str(name), nn_str(sig), NULL);
  return result;
}

static jmethodID JNICALL GetStaticMethodID(JNIEnv *env, jclass clazz, const char *name, const char *sig) {
  jmethodID result = jnifun->GetStaticMethodID(env, clazz, name, sig);
  jni_trace_call(env, "GetStaticMethodID", nn_class(clazz), nn_bool(result), nn_str(name), nn_str(sig), NULL);
  return result;
}

static jfieldID JNICALL GetFieldID(JNIEnv *env, jclass clazz, const char *name, const char *sig) {
  jfieldID result = jnifun->GetFieldID(env, clazz, name, sig);
  jni_trace_call(env, "GetFieldID", nn_class(clazz), nn_bool(result), nn_str(name), nn_str(sig), NULL);
  return result;
}

static jfieldID JNICALL GetStaticFieldID(JNIEnv *env, jclass clazz, const char *name, const char *sig) {
  jfieldID result = jnifun->GetStaticFieldID(env, clazz, name, sig);
  jni_trace_call(env, "GetStaticFieldID", nn_class(clazz), nn_bool(result), nn_str(name), nn_str(sig), NULL);
  return result;
}

void OnVMStart_JNI(jvmtiEnv *jvmti, JNIEnv *jni) {
  jniNativeInterface *functions;
  guarantee((*jvmti)->GetJNIFunctionTable(jvmti, &functions) == JVMTI_ERROR_NONE);
  functions->DefineClass = &DefineClass;
  functions->FindClass = &FindClass;
  functions->GetMethodID = &GetMethodID;
  functions->GetStaticMethodID = &GetStaticMethodID;
  functions->GetFieldID = &GetFieldID;
  functions->GetStaticFieldID = &GetStaticFieldID;
  guarantee((*jvmti)->SetJNIFunctionTable(jvmti, functions) == JVMTI_ERROR_NONE);
  guarantee((*jvmti)->Deallocate(jvmti, (unsigned char *) functions) == JVMTI_ERROR_NONE);
}

#ifdef __cplusplus
}
#endif
