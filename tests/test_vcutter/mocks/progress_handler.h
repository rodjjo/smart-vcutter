/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef TESTS_TEST_VCUTTER_MOCKS_PROGRESS_HANDLER_H_
#define TESTS_TEST_VCUTTER_MOCKS_PROGRESS_HANDLER_H_

#include "src/clippings/clipping_conversion.h"

class ProgressHandlerMock : public vcutter::ProgressHandler {
 public:
    bool wait(vcutter::progress_task_t task) override {
        return task();
    }

    bool canceled() override {
        return false;
    }

    void set_buffer(uint8_t *buffer, uint32_t w, uint32_t h) override {
    }

    void set_progress(uint32_t progress, uint32_t max_progress) override {
    }
};

#endif // TESTS_TEST_VCUTTER_MOCKS_PROGRESS_HANDLER_H_
