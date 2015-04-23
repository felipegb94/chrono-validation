/*
* BaseTest.h
*
*  Created on: Nov 23, 2014
*      Author: felipegb94
*/

#ifndef BaseTest_H_
#define BaseTest_H_

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstdio>

#define RAPIDJSON_HAS_STDSTRING 1
#include "../include/rapidjson/document.h"
#include "../include/rapidjson/stringbuffer.h"
#include "../include/rapidjson/prettywriter.h"
#include "../include/rapidjson/filewritestream.h"



class BaseTest {

public:

  bool        m_passed;      ///< Did the test pass or fail?

  /// Constructor: Every test has to have a name and an associated project to it.
  BaseTest(const std::string& testName, const std::string& testProjectName)
  : m_name(testName),
    m_projectName(testProjectName),
    m_passed(false)
  {
    // Initialize JSON object
    m_testJson.SetObject();
    rapidjson::Document::AllocatorType& allocator = m_testJson.GetAllocator(); ///< Allocates space in m_testjson.
 
    /// Rapidjson variables to populate and append to json object
    rapidjson::Value jsonName; 
    rapidjson::Value jsonProject;
    m_jsonMetrics.SetObject();

    // Set Values to json elements. Cast c strings to rapidjson proper string type.
    jsonName.SetString(rapidjson::StringRef(m_name.c_str()));
    jsonProject.SetString(rapidjson::StringRef(m_projectName.c_str()));

    // Add values to JSON File
    m_testJson.AddMember("name", jsonName, allocator);
    m_testJson.AddMember("project_name", jsonProject, allocator);

  }

  virtual ~BaseTest() {}

  /// Main function for running the test
  void run() {

    m_passed = execute();
    finalizeJson();
  }

  /// Add a test-specific metric (a key-value pair)
  void addMetric(const std::string& metricName,
                 double             metricValue) {

    rapidjson::Document::AllocatorType& allocator = m_testJson.GetAllocator(); ///< Allocates space in m_testjson.   
    rapidjson::Value metricNameV(metricName, allocator);
    rapidjson::Value metricValueV(metricValue);
    m_jsonMetrics.AddMember(metricNameV, metricValueV, allocator);

  }
  
  void addMetric(const std::string& metricName,
                 int                metricValue) {

    rapidjson::Document::AllocatorType& allocator = m_testJson.GetAllocator(); ///< Allocates space in m_testjson.
    rapidjson::Value metricNameV(metricName, allocator);
    rapidjson::Value metricValueV(metricValue);
    m_jsonMetrics.AddMember(metricNameV, metricValueV, allocator);
  }

  void addMetric(const std::string& metricName,
                 uint64_t                metricValue) {

    rapidjson::Document::AllocatorType& allocator = m_testJson.GetAllocator(); ///< Allocates space in m_testjson.
    rapidjson::Value metricNameV(metricName, allocator);
    rapidjson::Value metricValueV(metricValue);
    m_jsonMetrics.AddMember(metricNameV, metricValueV, allocator);
  }

  void addMetric(const std::string& metricName,
                 const std::string& metricValue) {

    rapidjson::Document::AllocatorType& allocator = m_testJson.GetAllocator(); ///< Allocates space in m_testjson.   
    rapidjson::Value metricNameV(metricName, allocator);
    rapidjson::Value metricValueV(metricValue, allocator);
    m_jsonMetrics.AddMember(metricNameV, metricValueV, allocator);
  }

  void addMetric(const std::string&   metricName,
                 std::vector<double>& metricValue) {
    // append to JSON document
    rapidjson::Document::AllocatorType& allocator = m_testJson.GetAllocator(); ///< Allocates space in m_testjson.   
    rapidjson::Value jsonMetricValue(rapidjson::kArrayType);

    for (std::vector<double>::iterator itr = metricValue.begin(); itr != metricValue.end(); ++itr){
      jsonMetricValue.PushBack(*itr, allocator);
    }

    rapidjson::Value metricNameV(metricName.c_str(), allocator);
    m_jsonMetrics.AddMember(metricNameV, jsonMetricValue, allocator);

  }
  

  /// Contains Test. Returns if test passed or failed
  virtual bool execute() = 0;

  /// Return total execution time for this test.
  virtual double getExecutionTime() const = 0;

  /// Print
  /// TODO: Create a more descriptive print.
  void print() {
    std::cout << "Test Information: " << std::endl;
    std::cout << "Test Name =  " << m_name << std::endl;
    std::cout << "Project Name = " << m_projectName << std::endl;
  };

private:

  std::string m_name;        ///< Name of test
  std::string m_projectName; ///< Name of the project: e.g: chrono, chronoRender, etc.

  /// Metrics that are being tested (key-value pairs)
  rapidjson::Value    m_jsonMetrics;
  rapidjson::Document m_testJson;     ///< JSON output of the test

  /// This function finalizaes the json object and writes to a file
  void finalizeJson() {

    rapidjson::Document::AllocatorType& allocator = m_testJson.GetAllocator(); ///< Allocates space in m_testjson.

    /// Add remaining results to json file.
    rapidjson::Value jsonPassed; ///< Did the test pass?
    rapidjson::Value jsonExecutionTime; ///< How long did it take the test to run.

    /// Set values to json elements
    jsonPassed.SetBool(m_passed);
    jsonExecutionTime.SetDouble(getExecutionTime());

    /// Add to json the values obtained through the test.
    m_testJson.AddMember("passed", jsonPassed, allocator);
    m_testJson.AddMember("execution_time", jsonExecutionTime, allocator);
    m_testJson.AddMember("metrics", m_jsonMetrics, allocator);

    // Write Json to file
    /* PATH in buildbotslave:
     /home/hammad/buildbot/slave/test_results/results
     */
    std::string filename =  "/Users/felipegb94/SBEL/results/" + m_name + ".json";
    char writeBuffer[65536];
    FILE* fp = fopen(filename.c_str(), "w"); // non-Windows use "w"

    rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
    rapidjson::Writer<rapidjson::FileWriteStream> writer(os);

    m_testJson.Accept(writer);

    fclose(fp);

  }

};



#endif // BaseTest_H_
