from flask import Flask, render_template, request, jsonify
from supabase import create_client, Client
import os
from datetime import datetime, timezone
from dotenv import load_dotenv

# Load environment variables
load_dotenv()

# Initialize Flask app
app = Flask(__name__)

# Supabase client setup
SUPABASE_URL = os.getenv('SUPABASE_URL')
SUPABASE_KEY = os.getenv('SUPABASE_KEY')
supabase: Client = create_client(SUPABASE_URL, SUPABASE_KEY)

# Current values (initial values, you can replace with actual subsystem)
current_ph = 7.0
current_temp = 25.0
current_stirring_speed = 10.0

# Route to display the UI
@app.route('/')
def index():
    return render_template('index.html', ph=current_ph, temp=current_temp, stirring_speed=current_stirring_speed)

# Route to handle updates from the UI
@app.route('/update', methods=['POST'])
def update():
    global current_ph, current_temp, current_stirring_speed
    
    # Get data from the request
    ph = request.form['ph']
    temp = request.form['temperature']
    stirring_speed = request.form['stirring_speed']
    
    # Update the values
    current_ph = float(ph)
    current_temp = float(temp)
    current_stirring_speed = float(stirring_speed)
    
    # Log the changes to Supabase with ISO formatted timestamp
    supabase.table('subsystem_logs').insert({
        'timestamp': datetime.now(timezone.utc).isoformat(),
        'ph': current_ph,
        'temperature': current_temp,
        'stirring_speed': current_stirring_speed
    }).execute()
    
    return jsonify({'status': 'success'}), 200

# Route to get current values
@app.route('/current', methods=['GET'])
def get_current_values():
    return jsonify({
        'ph': current_ph,
        'temperature': current_temp,
        'stirring_speed': current_stirring_speed
    })

# Run the app
if __name__ == '__main__':
    app.run(debug=True)