/*
 * SPDX-License-Identifier: Apache-2.0
 */

//===---------------- Concat.cpp - Lowering Concat Op -------------------===//
//
// Copyright 2019-2022 The IBM Research Authors.
//
// =============================================================================
//
// This file lowers the ONNX Print Signature Operator to Krnl dialect.
//
//===----------------------------------------------------------------------===//

#include "src/Conversion/ONNXToKrnl/ONNXToKrnlCommon.hpp"
#include "src/Dialect/Krnl/KrnlHelper.hpp"
#include "src/Dialect/ONNX/ONNXOps/ShapeHelper.hpp"

using namespace mlir;

namespace onnx_mlir {

struct ONNXPrintSignatureLowering : public ConversionPattern {
  ONNXPrintSignatureLowering(TypeConverter &typeConverter, MLIRContext *ctx)
      : ConversionPattern(
            typeConverter, ONNXPrintSignatureOp::getOperationName(), 1, ctx) {}

  LogicalResult matchAndRewrite(Operation *op, ArrayRef<Value> operands,
      ConversionPatternRewriter &rewriter) const final {
    // Gather info.
    Location loc = op->getLoc();
    MultiDialectBuilder<KrnlBuilder> create(rewriter, loc);
    ONNXPrintSignatureOp printSignatureOp =
        llvm::dyn_cast<ONNXPrintSignatureOp>(op);
    ONNXPrintSignatureOpAdaptor operandAdaptor(operands);

    std::string opName(printSignatureOp.getOpName().data());
    create.krnl.printf(opName);
    std::string msg = "%t ";
    for (Value oper : operandAdaptor.getInput())
      if (!oper.getType().isa<NoneType>())
        create.krnl.printTensor(msg, oper);
    Value noneValue;
    rewriter.replaceOpWithNewOp<KrnlPrintOp>(op, "\n", noneValue);
    return success();
  }
};

void populateLoweringONNXPrintSignaturePattern(RewritePatternSet &patterns,
    TypeConverter &typeConverter, MLIRContext *ctx) {
  patterns.insert<ONNXPrintSignatureLowering>(typeConverter, ctx);
}

} // namespace onnx_mlir
