from selenium import webdriver
from selenium.webdriver.chrome.options import Options
import time

options = Options()
options.add_argument("--headless") # Run in headless mode for the test
options.add_argument("--no-sandbox")
options.add_argument("--disable-dev-shm-usage")

try:
    print("Starting Chrome...")
    driver = webdriver.Chrome(options=options)
    print("Chrome started successfully!")
    driver.get("https://www.google.com")
    print(f"Page title: {driver.title}")
    driver.quit()
    print("Chrome closed.")
except Exception as e:
    print(f"Failed to start Chrome: {e}")
