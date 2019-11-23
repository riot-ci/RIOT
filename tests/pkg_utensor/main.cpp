/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       uTensor example application
 *
 * Adapted from the uTensor sample application by Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "models/deep_mlp.hpp"  //generated model file
#include "tensor.hpp"  //useful tensor classes
#include "blob/digit.h"  //contains a sample taken from the MNIST test set

int main(void)
{
    puts("Simple MNIST end-to-end uTensor cli example (device)\n");

    Context ctx;  //creating the context class, the stage where inferences take place
    //wrapping the input data in a tensor class
    Tensor* input_x = new WrappedRamTensor<float>({1, digit_len >> 2}, (float *)digit);

    get_deep_mlp_ctx(ctx, input_x);  // pass the tensor to the context
    S_TENSOR pred_tensor = ctx.get("y_pred:0");  // getting a reference to the output tensor
    ctx.eval(); //trigger the inference

    uint8_t pred_label = *(pred_tensor->read<int>(0, 0));  //getting the result back
    printf("Predicted label: %d\r\n", pred_label);

    return 0;
}
