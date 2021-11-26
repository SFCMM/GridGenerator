#ifndef LBM_CONFIGURATION_H
#define LBM_CONFIGURATION_H
#include <json.h>
using json = nlohmann::json;

class configuration {
 public:
  /// Set the configuration file name
  /// \param configFileName Configuration file name
  void setConfiguration(const GString& configFileName) { m_configFileName = configFileName; }

  /// Set the configuration from json-object
  /// \param config json-object to be used for configuration
  void setConfiguration(const json& config) {
    m_config = config;
    setupUnusedTracking();
  }

  /// Load the configuration from a configuration file based on the object
  /// \param section Section of the configuration file to use (optional default=everything)
  void loadConfiguration(const GString& section = "") {
    if(!m_config.empty()) {
      logger << "Warning loading configuration, but configuration already loaded!" << std::endl;
      TERMM(-1, "Invalid operation!");
    }
    logger << "Loading configuration file [" << configFile() << "]" << std::endl;

    // 1. open configuration file on root process
    if(MPI::isRoot() && isFile(configFile())) {
      std::ifstream configFileStream(configFile());
      configFileStream >> m_config;
      if(!section.empty()) {
        m_config = m_config[section];
      }
      setupUnusedTracking();
      configFileStream.close();
    }
  }

  /// Get a required configuration value (exit if it doesn't exist)
  /// \tparam T Type of the value
  /// \param key Key of the value
  /// \return Configuration value if it exist or exit
  template <typename T>
  auto required_config_value(const GString& key) -> T {
    // todo: check for types
    if(m_config.template contains(key)) {
      m_unusedKeys[key] = true;
      return static_cast<T>(m_config[key]);
    }
    TERMM(-1, "The required configuration value is missing: " + key);
  }

  /// Get an optional configuration value (return default value if value not found)
  /// \tparam T Type of the value
  /// \param key Key of the value
  /// \param defaultValue Default value
  /// \return Configuration value if it exists or the given default
  template <typename T>
  auto opt_config_value(const GString& key, const T& defaultValue) -> T {
    // todo: check for types
    if(m_config.template contains(key)) {
      m_unusedKeys[key] = true;
      return static_cast<T>(m_config[key]);
    }
    return defaultValue;
  }

  /// Check if a configuration value exists
  /// \param key Key of the value to check
  /// \return true if it exists
  auto has_config_value(const GString& key) -> GBool { return m_config.template contains(key); }

  /// Exists a key-value pair in the configuration?
  /// \param key The key of the value
  /// \param value The value
  /// \return The key-value pair exits -> true
  auto has_any_key_value(const GString& key, const GString& value) const -> GBool {
    auto has_config_value_key = [=](const json& cc, const GString& _key, const GString& val) {
      // entry exists
      if(cc.contains(_key)) {
        // value matches
        if(cc[_key] == val) {
          return true;
        }
      }
      return false;
    };

    std::stack<json> stack;
    stack.emplace(m_config);

    do {
      json tmp = stack.top();
      stack.pop();

      if(has_config_value_key(tmp, key, value)) {
        return true;
      }
      for(const auto& item : tmp) {
        if(item.is_object()) {
          stack.emplace(item);
        }
      }
    } while(!stack.empty());

    return false;
  }

  /// Get all the keys with a certain value
  /// \param value Value to search for
  /// \return List of keys with the value
  auto get_all_items_with_value(const GString& value) const -> std::vector<json> {
    auto has_config_value = [=](const json& cc, const GString& val) {
      // entry exists
      for(const auto& item : cc) {
        if(item == val) {
          return true;
        }
      }
      return false;
    };

    std::vector<json>   found;
    std::stack<json>    stack;
    std::stack<GString> keyStack;
    stack.emplace(m_config);
    keyStack.emplace("default");


    do {
      json tmp = stack.top();
      stack.pop();
      GString tmpK = keyStack.top();
      keyStack.pop();

      if(has_config_value(tmp, value)) {
        json hit;
        hit[tmpK] = tmp;
        found.emplace_back(hit);
        continue;
      }
      for(const auto& [key, v] : tmp.items()) {
        if(v.is_object()) {
          stack.emplace(v);
          keyStack.emplace(key);
        }
      }
    } while(!stack.empty());

    return found;
  }

  /// Print out a list of unused configuration values
  void unusedConfigValues() {
    GInt i = 0;
    logger << "The following values in the configuration file are unused: \n";
    for(const auto& configKey : m_unusedKeys) {
      if(!configKey.second) {
        logger << "[" << ++i << "] " << configKey.first << "\n";
      }
    }
    logger << std::endl;
  }

  /// Get the configuration file name
  /// \return Configuration file name
  auto configFile() const -> GString { return m_configFileName; }

  /// Get the json-object of the configuration
  /// \return JSON-object
  auto config() const -> const json& { return m_config; }

 private:
  /// Init the tracking of unused configuration values
  void setupUnusedTracking() {
    // put all available keys in map to keep track of usage
    for(const auto& element : m_config.items()) {
      m_unusedKeys.emplace(element.key(), false);
    }
  }

  // file name of the configuration
  GString m_configFileName = "grid.json";
  // json configuration object
  json m_config{};
  // map used for tracking the unused values
  std::unordered_map<GString, GBool> m_unusedKeys{};
};

#endif // LBM_CONFIGURATION_H