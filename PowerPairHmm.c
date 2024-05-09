/* Header files */

/* We need to include */
/* - jni.h to use JNI */
/* - math.h for NAN */
/* - stdlib.h to use src_c{calloc} and src_c{posix_memalign} */
/* - string.h for src_c{memset} */


/*
 * NOTE: THIS FILE IS GENERATED from an Org mode document. Edit the original
 * document and re-tangle this file.
 */

#include <jni.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

/* Typedefs */

/* Tracking pointer types is helped out by adding some typedefs. */


typedef jdouble *jdoubleP;
typedef jdoubleArray *jdoubleArrayP;

/* getDouble2dArray: Get access to double [][] arrays */

/* This C function */
/* - gets the size of the outer array using src_c{GetObjectLength} */
/* - allocates an array of src_c{jdoubleP} */
/* - allocates an array of src_c{jdoubleArray} to use later to release */
/*   memory */
/* - for each element of the outer array */
/*   - call src_c{GetDoubleArrayElements} to get access to the Java */
/*     src_java{double [][]} */
/*   - save the array src_c{jdoubleArray} and the native src_c{jdoubleP} */


jdoubleP *
getDouble2dArray(JNIEnv *env, jobjectArray matrix, jdoubleArrayP *arrays)
{



	/* Get the length of the outer array. */


	jsize len = GetArrayLength(env, matrix);



	/* Allocate src_c{jdoubleP} and src_c{jdoubleArray} arrays. We */
	/* use src_c{posix_memalign} for the src_c{jdoubleP} array to */
	/* help the compiled code use POWER vector instructions. */


	jdoubleP *ptrs;

	int rc = posix_memalign(&ptrs, 16, len * sizeof(jdoubleP));
	if (rc != 0) {
		return NULL;
	}
	(void) memset(ptrs, 0, len * sizeof(jdoubleP));

	*arrays = calloc(len, sizeof(jdoubleArray));
	if (*arrays == NULL) {
		goto cleanptrs;
	}



	/* For each index src_c{i} from 0 to src_c{len - 1} */
	/* - get outer array element, a src_c{jdoubleArray}. Save in src_c{*arrays[i]} */
	/* - call GetDoubleArrayElements on src_c{*arrays[i]}. Save in */
        /*   src_c{ptrs[i]} */


	for (jsize i = 0; i < len; i++) {
		*arrays[i] = GetObjectArrayElement(env, matrix, i);
		if (*arrays[i] == NULL) {
			goto cleanarrays;
		}
		ptrs[i] = GetDoubleArrayElements(env, *arrays[i], NULL);
		if (ptrs[i] == NULL) {
			goto cleanarrays;
		}
	}



	/* At this point, we're done, so return src_c{ptrs}. */


	return ptrs;



	/* Now we have the cleanup labels. Recall that the code jumps to */
	/* either src_c{cleanptrs:} or src_c{cleanarrays:} in the case of */
	/* errors. We handle these in reverse order, with */
	/* src_c{cleanarrays:} first and then falling through to */
	/* src_c{cleanptrs:}. */

	/* For src_c{cleanarrays:} we need to release memory back to */
	/* Java. Rely on the fact that we zeroed out src_c{ptrs} and */
	/* src_c{*arrays} when allocating memory to break out of the loop */
	/* early. Also free src_c{*arrays} here. */


cleanarrays:
	for (int i = 0; i < len; i++) {
		if (ptrs[i] == NULL) {
			break;
		}

		/* ReleaseDoubleArrayElements() with mode=0 frees ptrs[i] */
		ReleaseDoubleArrayElements(env, *arrays[i], ptrs[i], 0);
	}

	free(*arrays);



	/* src_c{cleanptrs:} only handles freeing src_c{*ptrs}, then */
	/* returns NULL. */


cleanptrs:
	free(ptrs);
	return NULL;
}

/* releaseDouble2dArray: release memory and flush to Java */

/* src_c{releaseDouble2dArray} is similar to the clean up code in */
/* src_c{getDouble2dArray}, but leaves out checks for NULL pointers as */
/* there shouldn't be any. */


void
releaseDouble2dArray(JNIEnv *env, jobjectArray matrix, jdoubleP *native, jdoubleArrayP *jarray)
{



	/* First, get the length of the array. */


	jsize len = GetArrayLength(matrix);



	/* For each index from 0 to src_c{len - 1} */
	/* - call ReleaseDoubleArrayElements with src_c{native[i]} and */
        /*   src_c{*jarray[i]} */


	for (int i = 0; i < len; i++) {
		/* ReleaseDoubleArrayElements with mode=0 frees memory */
		ReleaseDoubleArrayElements(env, *jarray[i], native[i], 0);
	}



	/* Now, free src_c{jarray} and src_c{native}. */


	free(*jarray);
	*jarray = NULL;
	free(native);
}

/* Native code and JNI */


/* The native code will use JNI to get access to Java class data. Since */
/* we have 5 matrices declared as src_java{double [][]}, we'll write a C */
/* function src_c{get2dDoubleArray} to set up access to these matrices. */

/* A src_java{double [][]} is implemented as an array of arrays of */
/* doubles. This is passed as a src_c{jObjectArray} to */
/* C/C++. src_c{GetObjectArrayLength} is called to get the length of the */
/* outer array. Then a array of double pointers (src_c{double *}) is */
/* dynamically allocated using src_c{malloc} or src_c++{new}. */

/* The native code for the main loop of */
/* src_java{subComputeReadLikelihoodGivenHaplotypeLog10} will start with */


JNIEXPORT jdouble JNICALL
Java_com_ibm_pairhmm_PowerPairHmm_subComputeReadLikelihoodGivenHaplotypeLog10Inner
(JNIEnv *env, jobject this, jint paddedReadLength, jint hapStartIndex,
 jint paddedHaplotypeLength, jobjectArray matchMatrix, jobjectArray priorMatrix,
 jobjectArray transitionMatrix, jobjectArray insertionMatrix,
 jobjectArray deletionMatrix)
{



	/* Recall in [[Second loop]] that src_java{hapStartIndex}, */
	/* src_java{paddedHaplotypeLength}, and src_java{paddedReadLength} are */
	/* all integer types. We create 3 C variables that use these */
	/* values cast to src_c{int}. */


	int start = (int) hapStartIndex;
	int hapLength = (int) paddedHaplotypeLength;
	int readLength = (int) paddedReadLength;



	/* We declare and initialize src_c{finalSumProbabilities} to */
	/* src_c{NAN} here so if there's a problem encountered before */
	/* computing src_c{finalSumProbabilities} we return src_c{NAN}. */


	jdouble finalSumProbabilities = NAN;


	
	/* The rest of the variables are src_c{jobjectArray}s that are Java */
	/* src_java{double [][]}. We will represent these as src_c{double **} */
	/* variables. */


	jdouble **match, **prior, **transition, **insertion, **deletion;



	/* Underneath these we have src_c{jdoubleArray}s that we will gain access */
	/* to via the JNI function src_c{GetDoubleArrayElements}. In order to */
	/* release our access when we're done, we'll need to pass the original */
	/* src_c{jdoubleArray} and the native type pointer to */
	/* src_c{ReleaseDoubleArrayElements()}. So we'll need to track both of */
	/* these together. We will track the src_c{jdoubleArray}s in separate */
	/* arrays, dynamically allocated. */


	jdoubleArray *jMatch, *jPrior, *jTransition, *jInsertion, *jDeletion;



	/* For each of the matrices, we call a new routine */
	/* src_c{getDouble2dArray} to retrieve the array elements and track them. */


	if ((match = getDouble2dArray(matchMatrix, &jMatch)) == NULL) {
		return NAN;
	}
	if ((prior = getDouble2dArray(priorMatrix, &jPrior)) == NULL) {
		goto free_match;
	}
	if ((transition = getDouble2dArray(transitionMatrix, &jTransition)) == NULL) {
		goto free_prior;
	}
	if ((insertion = getDouble2dArray(insertionMatrix, &jInsertion)) == NULL) {
		goto free_transition;
	}
	if ((deletion = getDouble2dArray(deletionMatrix, &jDeletion)) == NULL) {
		goto free_insertion;
	}



	/* If these calls succeed, we can now get on to the matrix manipulations. */


	for (int i = 1; i < readLength; i++) {
		/* +1 here is because hapStartIndex is 0-based, but our matrices are 1 */
		/* based */
		for (int j = start+1; j < hapLength; j++) {
			match[i][j] = prior[i][j] *
				( match[i - 1][j - 1] *
				  transition[i][matchToMatch] +
				  insertion[i - 1][j - 1] *
				  transition[i][indelToMatch] +
				  deletion[i - 1][j - 1] *
				  transition[i][indelToMatch] );
			insertion[i][j] = match[i - 1][j] *
				transition[i][matchToInsertion] +
				insertion[i - 1][j] *
				transition[i][insertionToInsertion];
			deletion[i][j] = match[i][j - 1] *
				transition[i][matchToDeletion] +
				deletion[i][j - 1] *
				transition[i][deletionToDeletion];
		}
	}



	/* Next we compute the final log probability. */


	/*
	 * final log probability is the log10 sum of the last element in the
	 * Match and Insertion state arrays
	 * this way we ignore all paths that ended in deletions! (huge)
	 * but we have to sum all the paths ending in the M and I matrices,
	 * because they're no longer extended.
	 */
        int endI = readLength - 1;
        finalSumProbabilities = 0.0;
        for (int j = 1; j < hapLength; j++) {
            finalSumProbabilities += matchMatrix[endI][j] + insertionMatrix[endI][j];
        }



	/* We next release all the memory for the matrices. */


	deletion = releaseDouble2dArray(deletionMatrix, deletion, &jDeletion);
free_insertion:
	insertion = releaseDouble2dArray(insertionMatrix, insertion, &jInsertion);
free_transition:
	transition = releaseDouble2dArray(transitionMatrix, transition, &jTransition);
free_prior:
	prior = releaseDouble2dArray(priorMatrix, prior, &jPrior);
free_match:
	match = releaseDouble2dArray(matchMatrix, match, &jMatch);



	/* Finally, we return src_c{finalSumProbabilities}. */


	return finalSumProbabilities;
}
