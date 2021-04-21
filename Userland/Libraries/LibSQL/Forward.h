/*
 * Copyright (c) 2021, Tim Flynn <trflynn89@pm.me>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

namespace SQL {
class ASTNode;
class BetweenExpression;
class BinaryOperatorExpression;
class BlobLiteral;
class CaseExpression;
class CastExpression;
class ChainedExpression;
class CollateExpression;
class ColumnDefinition;
class ColumnNameExpression;
class CommonTableExpression;
class CreateTable;
class Delete;
class DropTable;
class ErrorExpression;
class ErrorStatement;
class Expression;
class InChainedExpression;
class InTableExpression;
class InvertibleNestedDoubleExpression;
class InvertibleNestedExpression;
class IsExpression;
class Lexer;
class MatchExpression;
class NestedDoubleExpression;
class NestedExpression;
class NullExpression;
class NullLiteral;
class NumericLiteral;
class Parser;
class QualifiedTableName;
class ReturningClause;
class SignedNumber;
class Statement;
class StringLiteral;
class Token;
class TypeName;
class UnaryOperatorExpression;
}
