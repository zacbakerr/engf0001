from flask import Flask, render_template, request, jsonify
from supabase import create_client, Client
import os
from datetime import datetime, timezone
# from dotenv import load_dotenv

# Load environment variables
# load_dotenv()

# Initialize Flask app
app = Flask(__name__)

# Supabase client setup
# SUPABASE_URL = os.getenv('SUPABASE_URL')
# SUPABASE_KEY = os.getenv('SUPABASE_KEY')
SUPABASE_URL = 'https://yooxrfmtfsmlsbkvefte.supabase.co'
SUPABASE_KEY = 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6Inlvb3hyZm10ZnNtbHNia3ZlZnRlIiwicm9sZSI6ImFub24iLCJpYXQiOjE3MzI1NTM2MzgsImV4cCI6MjA0ODEyOTYzOH0.auzzcOuIFDvRiJQo01WWNTHTAOerGJrUkR5KF_KRlOA'
supabase: Client = create_client(SUPABASE_URL, SUPABASE_KEY)

# Route to display the UI
@app.route('/')
def index():
    response = supabase.table('subsystem_logs').select('*').order('timestamp', desc=True).limit(1).execute()
    
    if response.data:
        latest_entry = response.data[0]
        return render_template('index.html', 
                               ph=latest_entry['ph'], 
                               temp=latest_entry['temperature'], 
                               stirring_speed=latest_entry['stirring_speed'])
    else:
        return render_template('index.html', 
                               ph=999, 
                               temp=999, 
                               stirring_speed=999)

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
    response = supabase.table('subsystem_logs').select('*').order('timestamp', desc=True).limit(1).execute()
    
    if response.data:
        latest_entry = response.data[0]
        return jsonify({
            'ph': latest_entry['ph'],
            'temperature': latest_entry['temperature'],
            'stirring_speed': latest_entry['stirring_speed']
        })
    else:
        return jsonify({'error': 'No data found'}), 404

if __name__ == '__main__':
    app.run()