import selenium
print(f"Selenium version: {selenium.__version__}")
try:
    from selenium import webdriver
    print("Selenium webdriver imported successfully")
except ImportError:
    print("Failed to import selenium.webdriver")
