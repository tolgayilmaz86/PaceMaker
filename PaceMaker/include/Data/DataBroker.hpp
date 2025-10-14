#pragma once

#include <functional>
#include <unordered_map>
#include <concepts>

namespace pacemaker
{

/**
* DataBroker class template for managing data subscriptions and notifications.
* Uses observer pattern for real-time data streaming
*/
template<typename T>
class DataBroker
{
public:
  using SubscriptionId = size_t;
  using DataReceivedCallback = std::function<void(const T&)>;

  /**
   * @brief Subscribe to data updates
   * @param callback Function to call when new data is published
   * @return SubscriptionId Unique ID for the subscription, used for unsubscribing
   */
  [[nodiscard]] SubscriptionId Subscribe(DataReceivedCallback callback) {
    auto id = m_nextId++;
    m_subscribers[id] = std::move(callback);

    // Immediately send latest data to new subscriber
    if (m_subscribers.size() > 1)
    {
      callback(m_latestData);
    }

    return id;
  }

  /** @brief Unsubscribe from data updates
   *  @param id Subscription ID to unsubscribe
   */
  void Unsubscribe(SubscriptionId id) {
    m_subscribers.erase(id);
  }

  /**
   * @brief Publish new data to all subscribers
   *        E.g.:
   *        LeaderboardData data = testDataGenerator.GetLeaderboardData();
   *        leaderboardBroker.Publish(data);  // Copy version - 'data' is still valid
   * @param data The data to publish
   */
  void Publish(const T& data) {
    m_latestData = data;

    for (const auto& [id, callback] : m_subscribers)
    {
      callback(data);
    }
  }

  /**
   * @brief Publish new data to all subscribers, move based for efficiency
   *        E.g.:
   *        broker.Publish(dataSource.GetLatestData());  // Move version
   *        Or explicit move:
   *        LeaderboardData data = testDataGenerator.GetLeaderboardData();
   *        leaderboardBroker.Publish(std::move(data));  // Move version - 'data' is now empty/invalid
   * @param data The data to publish
   */
  void Publish(T&& data) {
    m_latestData = std::move(data);

    for (const auto& [id, callback] : m_subscribers)
    {
      callback(m_latestData);
    }
  }

  /**
   * @brief Access the latest published data
   * @return const T& Reference to the latest published data
   */
  [[nodiscard]] const T& GetLatest() const noexcept { return m_latestData; }

  /**
   * @brief Access the number of current subscribers
   * @return size_t Number of subscribers
   */
  [[nodiscard]] size_t SubscriberCount() const noexcept { return m_subscribers.size(); }

private:
  std::unordered_map<SubscriptionId, DataReceivedCallback> m_subscribers; // Map of subscription ID to callback
  SubscriptionId m_nextId{ 0 }; // Incremental ID generator
  T m_latestData{}; // Latest published data
};
} // namespace pacemaker
