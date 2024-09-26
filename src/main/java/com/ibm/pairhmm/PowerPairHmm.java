/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2023 Intel Corporation
 * Copyright (c) 2024 IBM Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
package com.ibm.pairhmm;

import org.apache.commons.logging.LogFactory;
import org.apache.commons.logging.Log;

import org.broadinstitute.gatk.nativebindings.pairhmm.HaplotypeDataHolder;
import org.broadinstitute.gatk.nativebindings.pairhmm.PairHMMNativeArguments;
import org.broadinstitute.gatk.nativebindings.pairhmm.PairHMMNativeBinding;
import org.broadinstitute.gatk.nativebindings.pairhmm.ReadDataHolder;

import java.io.File;

/**
 * Provides a native PairHMM implementation accelerated for the Intel Architecture.
 */
public class PowerPairHmm implements PairHMMNativeBinding {

    private final static Log logger = LogFactory.getLog(PowerPairHmm.class);
    private static final String NATIVE_LIBRARY_NAME = "power_pairhmm";
    private String nativeLibraryName = "power_pairhmm";

    /**
     * Loads the native library, if it is supported on this platform. <p>
     *
     * @param tempDir  directory where the native library is extracted or null to use the system temp directory
     * @return  true
     */
    @Override
    public synchronized boolean load(File tempDir) {
        System.loadLibrary(nativeLibraryName);
	return true;
    }

    /**
     * Initialize native PairHMM with the supplied args.
     *
     * @param args the args used to configure native PairHMM
     */
    public void initialize(PairHMMNativeArguments args) {
        if (args == null) {
            args = new PairHMMNativeArguments();
            args.useDoublePrecision = false;
            args.maxNumberOfThreads = 1;
        }

        initNative(ReadDataHolder.class, HaplotypeDataHolder.class, args.useDoublePrecision, args.maxNumberOfThreads);

        // log information about threads
        int reqThreads = args.maxNumberOfThreads;
	if (reqThreads != 1) {
	    logger.warn(String.format("Ignoring request for %d threads; not using OpenMP implementation", reqThreads));
	}
    }

    /**
     *
     *
     * @param readDataArray array of read data
     * @param haplotypeDataArray array of haplotype data
     * @param likelihoodArray array of double results
     */

    @Override
    public void computeLikelihoods(ReadDataHolder[] readDataArray,
                                   HaplotypeDataHolder[] haplotypeDataArray,
                                   double[] likelihoodArray) throws NullPointerException, OutOfMemoryError, IllegalArgumentException
    {
        if(readDataArray == null || haplotypeDataArray == null || likelihoodArray == null) {
            throw new NullPointerException("Input is null");
        }
        try {
            computeLikelihoodsNative(readDataArray, haplotypeDataArray, likelihoodArray);
        } catch (OutOfMemoryError e) {
            logger.warn("Exception thrown from native PairHMM computeLikelihoodsNative function call " + e.getMessage());
            throw new OutOfMemoryError("Memory allocation failed");
        } catch (IllegalArgumentException e) {
            logger.warn("Exception thrown from native PairHMM computeLikelihoodsNative function call " + e.getMessage());
            throw new IllegalArgumentException("Ran into invalid argument issue");
        }

    }

    /**
     *
     */
    @Override
    public void done() {
        doneNative();
    }

    private native static void initNative(Class<?> readDataHolderClass,
                                          Class<?> haplotypeDataHolderClass,
                                          boolean doublePrecision,
                                          int maxThreads);

    private native void computeLikelihoodsNative(Object[] readDataArray,
                                                 Object[] haplotypeDataArray,
                                                 double[] likelihoodArray);

    private native void doneNative();
}
