#include <iostream>
#include <prometheus/counter.h>
#include <prometheus/exposer.h>
#include <prometheus/registry.h>

namespace Metrics {

// It should be initialized with make_shared<T>()
std::shared_ptr<prometheus::Registry> registry;
// All uses should be after the initial call to InitCounters
prometheus::Family<prometheus::Counter> *requests_counter;

// Call once in main function
void InitCounters() {
    registry = std::make_shared<prometheus::Registry>();
    requests_counter = &prometheus::BuildCounter().Name("requests_counter").Register(*Metrics::registry);
}

} // end of namespace Metrics

