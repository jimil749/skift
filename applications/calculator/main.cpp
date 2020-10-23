#include <libutils/NumberFormat.h>
#include <libutils/StringBuilder.h>
#include <libwidget/Application.h>
#include <libwidget/Markup.h>
#include <libwidget/Widgets.h>

#include "calculator/Calculator.h"

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = window_create_from_file("/Applications/calculator/calculator.markup");

    Calculator calculator{};

    auto calculator_observer = calculator.observe([&](Calculator &calculator) {
        window->with_widget<Label>("screen", [&](Label *label) {
            StringBuilder builder{};
            NumberFormat::decimal().format(builder, calculator.screen());
            label->text(builder.finalize());
        });
    });

    for (int i = 0; i < 10; i++)
    {
        char button_name[10];
        snprintf(button_name, 10, "button_%d", i);

        window->with_widget<Button>(button_name, [&](auto button) {
            button->on(Event::ACTION, [&, i](auto) { calculator.enter_number(i); });
        });
    }

    window->with_widget<Button>("button_plus", [&](auto button) {
        button->on(Event::ACTION, [&](auto) { calculator.enter_operation(Operation::ADD); });
    });

    window->with_widget<Button>("button_minus", [&](auto button) {
        button->on(Event::ACTION, [&](auto) { calculator.enter_operation(Operation::SUBSTRACT); });
    });

    window->with_widget<Button>("button_plus", [&](auto button) {
        button->on(Event::ACTION, [&](auto) { calculator.enter_operation(Operation::ADD); });
    });

    window->with_widget<Button>("button_minus", [&](auto button) {
        button->on(Event::ACTION, [&](auto) { calculator.enter_operation(Operation::SUBSTRACT); });
    });

    window->with_widget<Button>("button_multiply", [&](auto button) {
        button->on(Event::ACTION, [&](auto) { calculator.enter_operation(Operation::MULTIPLY); });
    });

    window->with_widget<Button>("button_divide", [&](auto button) {
        button->on(Event::ACTION, [&](auto) { calculator.enter_operation(Operation::DIVIDE); });
    });

    window->with_widget<Button>("button_reciprocal", [&](auto button) {
        button->on(Event::ACTION, [&](auto) { calculator.enter_operation(Operation::RECIPROCAL); });
    });

    window->with_widget<Button>("button_sqrt", [&](auto button) {
        button->on(Event::ACTION, [&](auto) { calculator.enter_operation(Operation::SQRT); });
    });

    window->with_widget<Button>("button_exp", [&](auto button) {
        button->on(Event::ACTION, [&](auto) { calculator.enter_operation(Operation::POWER); });
    });

    window->with_widget<Button>("button_equal", [&](auto button) {
        button->on(Event::ACTION, [&](auto) { calculator.equal(); });
    });

    window->with_widget<Button>("button_clear", [&](auto button) {
        button->on(Event::ACTION, [&](auto) { calculator.clear(); });
    });

    window->with_widget<Button>("button_clear_all", [&](auto button) {
        button->on(Event::ACTION, [&](auto) { calculator.clear_all(); });
    });

    window->show();

    return application_run();
}
