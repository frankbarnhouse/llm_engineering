from litellm import completion
from dotenv import load_dotenv
import os
import warnings

load_dotenv(override=True)

# Try without suppressing first
print("--- Running without suppression ---")
messages = [{"role": "system", "content": "You are a helpful assistant"}, {"role": "user", "content": "Hello"}]
try:
    response = completion(
        messages=messages, 
        model="groq/llama-3.1-8b-instant",
        reasoning_effort="low"
    )
    print("Success")
    # Accessing fields to see if it triggers
    _ = response.choices[0].message.content
except Exception as e:
    print(f"Error: {e}")

# Now try with suppression
print("\n--- Running with suppression ---")
warnings.filterwarnings("ignore", category=UserWarning, message=".*PydanticSerializationUnexpectedValue.*")

try:
    response = completion(
        messages=messages, 
        model="groq/llama-3.1-8b-instant",
        reasoning_effort="low"
    )
    print("Success with suppression")
except Exception as e:
    print(f"Error: {e}")
