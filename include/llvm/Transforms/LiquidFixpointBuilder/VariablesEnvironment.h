#ifndef LLVM_TRANSFORMS_UTILS_LIQUID_VARIABLESENVIRONMENT_H
#define LLVM_TRANSFORMS_UTILS_LIQUID_VARIABLESENVIRONMENT_H

#include <map>
#include <string>
#include <set>
#include <vector>
#include "llvm/Transforms/LiquidFixpointBuilder/FixpointConstraintBuilder.h"
#include "llvm/Transforms/LiquidFixpointBuilder/FixpointType.h"
#include "llvm/Transforms/LiquidFixpointBuilder/FunctionBlockGraph.h"

using namespace std::literals::string_literals;

namespace liquid
{

	class PhiNodeObligation
	{
	public:
		const std::string VariableSource;
		const std::string TargetFutureVariable;

		PhiNodeObligation() {}
		PhiNodeObligation(const std::string variableSource, const std::string targetFutureVariable) : VariableSource(variableSource), TargetFutureVariable(targetFutureVariable) {}
	};

	class VariablesEnvironment
	{
	private:
		std::string currentBlockName = ""s;
		const FunctionBlockGraph& functionBlockGraph;

		std::map<std::string, FixpointType> variableTypes;
		std::map<std::string, Counter> mutableVariableState;
		std::map<std::string, std::vector<std::string>> mutableVariableConstraints;
		std::map<std::string, std::map<std::string, std::string>> variablesMappingsPerBlock;
		std::map<std::string, std::set<std::string>> variablesValuesPerBlock;
		std::set<std::string> finishedBlocks;
		std::map<std::string, std::vector<PhiNodeObligation>> phiNodeObligations;

		std::set<std::string> outputVariables;
		std::set<std::string> mutableVariables;

		std::map<std::string, std::string> cachedBlockGuards;

		ResultType createVariable(const std::string& variable, const std::string& mappedVariableName, const FixpointType& type, const std::vector<std::string>& constraints, const std::string& expression);
		ResultType createIOVariable(const std::string& variable, const FixpointType& type, const std::vector<std::string>& constraints);
		ResultType validateFinishedAndUnfinishedBlockStates(const std::vector<std::string>& previousFinishedBlocks, const std::vector<std::string>& previousUnfinishedBlocks);
		ResultType getCommonVariables(std::vector<std::string> previousBlocks, std::set<std::string>& commonVariables);
		std::string getNextVariableName(const std::string& variable);
		std::vector<std::string> getBlockBinders(const std::string& blockName);
		ResultType getBlockBindersForPhiNode(const std::string& previousBlock, const std::string& blockName, const std::string& mappedVariableName, std::vector<std::string>& binders);
		ResultType createPhiNodeWithoutCreatedBinders(const std::string& variable, const std::string& mappedVariableName, const FixpointType& type, const std::vector<std::string>& sourceVariableNames, const std::vector<std::string>& previousBlocks);
		ResultType createPhiNodeInternal(const std::string& variable, const std::string& mappedVariableName, const FixpointType& type, const std::vector<std::string>& sourceVariableNames, const std::vector<std::string>& previousBlocks);

		ResultType getBlockGuard(const std::string& blockName, std::string& blockGuard);
		ResultType addVariableToBlockAndSuccessors(const std::string& blockName, const std::string& transitionGuardName);
		ResultType initializeBlockGuards();
		ResultType endBlock(const std::string& blockName);

	public:
		Counter FreshState;
		FixpointConstraintBuilder constraintBuilder;

		VariablesEnvironment(const FunctionBlockGraph& _functionBlockGraph) : functionBlockGraph(_functionBlockGraph) {}
		ResultType StartBlock(const std::string& blockName);
		ResultType CreateImmutableInputVariable(const std::string& variable, const FixpointType& type, const std::vector<std::string>& constraints);
		ResultType CreateMutableOutputVariable(const std::string& variable, const FixpointType& type, const std::vector<std::string>& constraints);
		ResultType CreateImmutableVariable(std::string variable, FixpointType type, std::vector<std::string> constraints, std::string expression);
		ResultType CreateMutableVariable(std::string variable, FixpointType type, std::vector<std::string> constraints, std::string expression);
		ResultType AssignMutableVariable(std::string variable, std::string expression);
		ResultType AddJumpInformation(const std::string& targetBlock);
		ResultType AddBranchInformation(const std::string& booleanVariable, const bool variableValue, const std::string& targetBlock);
		ResultType CreatePhiNode(const std::string& variable, const FixpointType& type, const std::vector<std::string>& sourceVariableNames, const std::vector<std::string>& previousBlocks);
		bool IsVariableDefined(std::string variableName);
		std::string GetVariableName(std::string variableName);
		std::string GetVariableAddress(std::string variableName);
		ResultType ToStringOrFailure(std::string& output);
	};
}

#endif
