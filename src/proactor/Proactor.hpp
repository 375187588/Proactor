/**
 * @file Proactor.hpp
 * @author Ronald T. Fernandez
 * @version 1.0
 */

#ifndef PROACTOR_PROACTOR_HPP_
#define PROACTOR_PROACTOR_HPP_

#include <condition_variable>
#include <memory>
#include <mutex>
#include "../completionEventQueue/CompletionEventQueue.hpp"
#include "../logger/Logger.hpp"
#include "../observer/Observer.hpp"
#include "../utils/Utils.hpp"

namespace proactor {
namespace proactor {

/**
 * This is the Proactor. Its mission is dequeuing completion events and then
 * notifying it to the IniitiatorCompletion handler.
 */
template <typename T>
class Proactor {
private:
	completionEventQueue::CompletionEventQueue<T> *completionEventQueue;
	observer::Observer<asyncOperation::AsynchronousOperation<T> > *observer;
	bool finish;
public:
	Proactor(completionEventQueue::CompletionEventQueue<T> *completionEventQueue, observer::Observer<asyncOperation::AsynchronousOperation<T> > *observer) :
		completionEventQueue(completionEventQueue), observer(observer) , finish(false) {};

	void canFinish(bool finish) {
		this->finish = finish;
	}

	void exec() {
		std::mutex m;
		std::condition_variable cv;

		while(!finish) {
			std::unique_lock<std::mutex> locker(m);
			while ((completionEventQueue->size() == 0) && (!finish) ) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			}
			if (completionEventQueue->size() > 0) {
				logger::Logger::log("Proactor removes  element from queue (size: " + utils::Utils::tostr(completionEventQueue->size()) + ")...");
				asyncOperation::AsynchronousOperation<T>* myoperation = completionEventQueue->pop();
				observer->notify(myoperation, myoperation->getId());
			}
		}
		std::cout <<"Proactor execution finished." << std::endl;
	};

	virtual ~Proactor() {
		logger::Logger::log("Finished Proactor.");
	};
};

} /* namespace proactor */
} /* namespace proactor */

#endif /* PROACTOR_PROACTOR_HPP_ */
