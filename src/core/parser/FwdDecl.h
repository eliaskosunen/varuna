/*
Copyright (C) 2016 Elias Kosunen

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <string>

namespace core
{
	namespace parser
	{
		class AST;

		class ASTNode;

		class ASTExpression;
		class ASTCallExpression;
		class ASTCastExpression;
		class ASTIdentifierExpression;

		class ASTLiteralExpression;
		class ASTIntegerLiteralExpression;
		class ASTFloatLiteralExpression;
		class ASTStringLiteralExpression;
		class ASTCharLiteralExpression;
		class ASTBoolLiteralExpression;
		class ASTNoneLiteralExpression;

		class ASTBinaryOperationExpression;
		class ASTUnaryOperationExpression;
		class ASTAssignmentOperationExpression;

		class ASTStatement;
		class ASTBlockStatement;
		class ASTVariableDefinitionExpression;

		class ASTIfStatement;
		class ASTForStatement;
		class ASTForeachStatement;
		class ASTWhileStatement;
		class ASTImportStatement;

		class ASTFunctionDeclarationStatement;
		class ASTFunctionDefinitionStatement;
		class ASTReturnStatement;

		class Visitor;

		class Parser;
	}
}
