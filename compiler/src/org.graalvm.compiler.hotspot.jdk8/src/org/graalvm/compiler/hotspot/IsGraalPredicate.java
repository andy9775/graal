/*
 * Copyright (c) 2018, Oracle and/or its affiliates. All rights reserved.
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
package org.graalvm.compiler.hotspot;

import static jdk.vm.ci.hotspot.HotSpotJVMCICompilerFactory.CompilationLevelAdjustment.ByHolder;
import static jdk.vm.ci.hotspot.HotSpotJVMCICompilerFactory.CompilationLevelAdjustment.None;

import jdk.vm.ci.hotspot.HotSpotJVMCICompilerFactory.CompilationLevelAdjustment;

/**
 * Determines if a given class is a JVMCI or Graal class for the purpose of
 * {@link HotSpotGraalCompilerFactory.Options#CompileGraalWithC1Only}.
 */
class IsGraalPredicate extends IsGraalPredicateBase {
    private final ClassLoader jvmciLoader = getClass().getClassLoader();

    @Override
    CompilationLevelAdjustment getCompilationLevelAdjustment() {
        return jvmciLoader != null ? ByHolder : None;
    }

    @Override
    boolean apply(Class<?> declaringClass) {
        assert jvmciLoader != null;
        // When running with +UseJVMCIClassLoader all classes loaded
        // by the JVMCI loader are considered to be Graal classes.
        try {
            if (declaringClass.getClassLoader() == jvmciLoader) {
                return true;
            }
        } catch (SecurityException e) {
            // This is definitely not a JVMCI or Graal class
        }
        return false;
    }
}
