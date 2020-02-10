#ifndef CPLEX_UTILS_H
#define CPLEX_UTILS_H

//-Wignored-attributes
// save diagnostic state
#pragma GCC diagnostic push 

//#pragma GCC diagnostic ignored "-Wignored-attributes"
//#pragma GCC diagnostic ignored "-Wall"
#pragma GCC system_header

#define IL_STD 1
#include <ilcplex/ilocplex.h>

// turn the warnings back on
#pragma GCC diagnostic pop


typedef enum __cut_policy { NO_CUTS, ONLY_ROOT_CUTS, FULL_CUTS } cut_policy;



// Documentation on 2D and 3D var arrays
// http://www-01.ibm.com/support/docview.wss?uid=swg21399963
#define CPLEX_2D_VAR_ARRAY( env, var_name, nlines, ncols, var_dec ) \
	IloArray<IloNumVarArray> var_name( env, nlines); \
	for( int l=0; l<nlines; l++ ) { \
		var_name[l] = IloNumVarArray(env, ncols); \
	    for( int c=0; c<ncols;  c++ ) var_name[l][c] = var_dec; \
	}


// Documentation on disabling Cplex Cuts
// http://www.ibm.com/support/knowledgecenter/SSSA5P_12.2.0/ilog.odms.cplex.help/Content/Optimization/Documentation/CPLEX/_pubskel/CPLEX562.html
// http://www.ibm.com/support/knowledgecenter/SS9UKU_12.5.0/com.ibm.cplex.zos.help/Parameters/topics/CutPass.html
#define CPLEX_DISABLE_CUTS( cplex ) { \
	cplex.setParam(IloCplex::CutPass, -1); \
	cplex.setParam(IloCplex::Cliques, -1); \
	cplex.setParam(IloCplex::Covers,  -1); \
	cplex.setParam(IloCplex::DisjCuts, -1); \
	cplex.setParam(IloCplex::FlowCovers, -1); \
	cplex.setParam(IloCplex::FlowPaths, -1); \
	cplex.setParam(IloCplex::FracCuts, -1); \
	cplex.setParam(IloCplex::GUBCovers, -1); \
	cplex.setParam(IloCplex::ImplBd, -1); \
	cplex.setParam(IloCplex::MIRCuts, -1); \
	cplex.setParam(IloCplex::ZeroHalfCuts, -1); \
	cplex.setParam(IloCplex::MCFCuts, -1); \
}


// Documentation on disabling presover
// http://www.ibm.com/support/knowledgecenter/SS9UKU_12.5.0/com.ibm.cplex.zos.help/UsrMan/topics/discr_optim/mip/para/48_preproc.html
// http://www.ibm.com/support/knowledgecenter/SS9UKU_12.5.0/com.ibm.cplex.zos.help/Parameters/topics/RepeatPresolve.html
#define CPLEX_DISABLE_PRESOLVER( cplex ) { \
	cplex.setParam(IloCplex::RepeatPresolve, 0); \
	cplex.setParam(IloCplex::AggInd,  0); \
	cplex.setParam(IloCplex::PreInd,  false); \
}


// Documentation Statement-Expressions
// http://stackoverflow.com/questions/2679182/have-macro-return-a-value
// https://gcc.gnu.org/onlinedocs/gcc-4.9.3/gcc/Statement-Exprs.html#Statement-Exprs
#define LP_CUT_LOOP( env, cplex, model, relax, cut_function ) ({ \
\
	int cuts_added; \
	double final_solution; \
\
	IloExtractable lp_conversion = IloExtractable( IloConversion(env, relax, ILOFLOAT)); \
	model.add( lp_conversion ); \
\
	do { \
		cuts_added = 0; \
		cplex.solve(); \
		cuts_added = cut_function; \
	} while( cuts_added > 0 ); \
\
	final_solution = cplex.getObjValue(); \
	model.remove( lp_conversion ); \
	final_solution; \
})


// Documentation Statement-Expressions
// http://stackoverflow.com/questions/2679182/have-macro-return-a-value
// https://gcc.gnu.org/onlinedocs/gcc-4.9.3/gcc/Statement-Exprs.html#Statement-Exprs
#define SOLVE_LP( env, cplex, model, relax ) ({ \
\
	double final_solution; \
	IloExtractable lp_conversion = IloExtractable( IloConversion(env, relax, ILOFLOAT)); \
	model.add( lp_conversion ); \
\
	cplex.solve(); \
	final_solution = cplex.getObjValue(); \
	model.remove( lp_conversion ); \
	final_solution; \
})


// NAME_VARS( 10, x, "x_%d" );
// NAME_VARS( C, z, "z_%d" );
#define NAME_VARS( array_size, var, format_string ) {\
	char buffer[100]; \
	for( int i = 0; i < array_size; i++ ){ \
		sprintf(buffer, format_string, i ); \
		var[i].setName( buffer ); \
	} \
}


// NAME_2D_VARS( 10, 20, x, "x_%d_%d" );
// NAME_2D_VARS( N, N, y, "y_%d_%d" );
#define NAME_2D_VARS( array_size1, array_size2, var, format_string ) {\
	char buffer[100]; \
	for( int i = 0; i < array_size1; i++ ){ \
		for( int j = 0; j < array_size2; j++ ){ \
			sprintf(buffer, format_string, i, j ); \
			var[i][j].setName( buffer ); \
		} \
	} \
}


#define CPLEX_MUTE( cplex ) {\
	cplex.setWarning( env.getNullStream() ); \
    cplex.setOut( env.getNullStream() ); \
}



#endif
