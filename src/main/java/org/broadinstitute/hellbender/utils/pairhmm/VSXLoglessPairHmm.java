// New Java class VSXLoglessPairHMM

// This code will be in GATK, in package
// org.broadinstitute.hellbender.utils.pairhmm.

// This code creates a new class src_java{VSXLoglessPairHMM} based on
// src_java{LoglessPairHMM}. It overrides
// src_java{subComputeReadLikelihoodGivenHaplotypeLog10} in order to call
// the native code.


package org.broadinstitute.hellbender.utils.pairhmm;



// Import src_java{VSXPairHMM}.


import com.ibm.power.pairhmm;

public final class VSXLoglessPairHMM extends LoglessPairHMM {

// Implement subComputeReadLikelihoodGivenHaplotypeLog10Native

//     src_java{class VSXLoglessPairHMM} will implement
//     src_java{subComputeReadLikelihoodGivenHaplotypeLog10} in order to call
//     the native code.

// #+attr_latex: :options \footnotesize

    ///
    /// {@inheritDoc}
    ///
    public double subComputeReadLikelihoodGivenHaplotypeLog10( final byte[] haplotypeBases,
                                                               final byte[] readBases,
                                                               final byte[] readQuals,
                                                               final byte[] insertionGOP,
                                                               final byte[] deletionGOP,
                                                               final byte[] overallGCP,
                                                               final int hapStartIndex,
                                                               final boolean recacheReadValues,
                                                               final int nextHapStartIndex) {



//         We leave initialization code in place.

// #+attr_latex: :options \footnotesize

        if (previousHaplotypeBases == null || previousHaplotypeBases.length != haplotypeBases.length) {
            final double initialValue = INITIAL_CONDITION / haplotypeBases.length;
            // set the initial value (free deletions in the beginning) for the first row in the
	    // deletion matrix
            for( int j = 0; j < paddedHaplotypeLength; j++ ) {
                deletionMatrix[0][j] = initialValue;
            }
        }

        if ( ! constantsAreInitialized || recacheReadValues ) {
            initializeProbabilities(transition, insertionGOP, deletionGOP, overallGCP);

            // note that we initialized the constants
            constantsAreInitialized = true;
        }

        initializePriors(haplotypeBases, readBases, readQuals, hapStartIndex);



        // Now we call our native method
        // src_java{subComputeReadLikelihoodGivenHaplotypeLog10Native}:


	double finalSumProbabilities;
	finalSumProbabilities =
	    subComputeReadLikelihoodGivenHaplotypeLog10Native(paddedReadLength,
							      hapStartIndex,
							      paddedHaplotypeLength,
							      matchMatrix,
							      prior,
							      transition,
							      insertion,
							      deletion);



        // The native method returns src_c{NAN} (Not A Number) if it
        // encounters errors. Check for that here and return if
        // detected.[fn:2]


	if (finalSumProbabilities.isNan()) {
	    return finalSumProbabilities;
	}



        // Finally, return log10 of src_java{finalSumProbabilities} minus
        // src_java{INITIAL_CONDITIION_LOG10}.


        return Math.log10(finalSumProbabilities) - INITIAL_CONDITION_LOG10;
    }

// Declare native methods

//     Here we add the src_java{private native} methods.


    private native static void
	subComputeReadLikelihoodGivenHaplotypeLog10Native(int paddedReadLength,
							  int hapStartIndex,
							  int paddedHaplotypeLength,
							  double [][] matchMatrix,
							  double [][] prior,
							  double [][] transition,
							  double [][] insertion,
							  double [][] deletion);
}
