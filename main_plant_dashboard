import React, { useState, useEffect, useCallback, useMemo } from 'react';
import { 
  Droplet, 
  Thermometer, 
  Leaf, 
  Settings, 
  RefreshCw, 
  Wifi, 
  WifiOff, 
  AlertCircle,
  Clock,
  Database
} from 'lucide-react';

// --- Components ---

const CircularProgress = ({ value, size = 120, strokeWidth = 12, color = 'text-green-500' }) => {
  const radius = (size - strokeWidth) / 2;
  const circumference = radius * 2 * Math.PI;
  const offset = circumference - (value / 100) * circumference;

  return (
    <div className="relative flex items-center justify-center" style={{ width: size, height: size }}>
      {/* Background Circle */}
      <svg className="absolute transform -rotate-90 w-full h-full">
        <circle
          cx={size / 2}
          cy={size / 2}
          r={radius}
          stroke="currentColor"
          strokeWidth={strokeWidth}
          fill="transparent"
          className="text-slate-100 dark:text-slate-700"
        />
        {/* Progress Circle */}
        <circle
          cx={size / 2}
          cy={size / 2}
          r={radius}
          stroke="currentColor"
          strokeWidth={strokeWidth}
          fill="transparent"
          strokeDasharray={circumference}
          strokeDashoffset={offset}
          strokeLinecap="round"
          className={`${color} transition-all duration-1000 ease-in-out`}
        />
      </svg>
      <div className="absolute flex flex-col items-center justify-center">
        <span className="text-3xl font-bold text-slate-800 dark:text-white">{Math.round(value)}%</span>
        <span className="text-xs text-slate-500 font-medium uppercase tracking-wider">Moisture</span>
      </div>
    </div>
  );
};

const PlantCard = ({ plant }) => {
  // Determine status color based on moisture level
  let statusColor = 'text-green-500';
  let bgColor = 'bg-green-100 dark:bg-green-900/30';
  let statusText = 'Optimal';

  if (plant.moisture < 30) {
    statusColor = 'text-red-500';
    bgColor = 'bg-red-100 dark:bg-red-900/30';
    statusText = 'Needs Water';
  } else if (plant.moisture > 80) {
    statusColor = 'text-blue-500';
    bgColor = 'bg-blue-100 dark:bg-blue-900/30';
    statusText = 'Overwatered';
  }

  // Format the last updated time
  const timeAgo = (dateString) => {
    if (!dateString) return 'Unknown';
    const date = new Date(dateString);
    const now = new Date();
    const seconds = Math.floor((now - date) / 1000);
    
    if (seconds < 60) return `${seconds}s ago`;
    if (seconds < 3600) return `${Math.floor(seconds / 60)}m ago`;
    return `${Math.floor(seconds / 3600)}h ago`;
  };

  return (
    <div className="bg-white dark:bg-slate-800 rounded-3xl p-6 shadow-sm border border-slate-100 dark:border-slate-700 transition-all hover:shadow-md">
      <div className="flex justify-between items-start mb-6">
        <div>
          <h3 className="text-xl font-bold text-slate-800 dark:text-white flex items-center gap-2">
            <Leaf className="w-5 h-5 text-emerald-500" />
            {plant.name || `Plant ${plant.id}`}
          </h3>
          <p className={`text-sm font-medium px-2 py-1 rounded-full mt-2 inline-block ${bgColor} ${statusColor}`}>
            {statusText}
          </p>
        </div>
        <div className="text-right">
          <div className="flex items-center text-slate-400 text-xs gap-1 justify-end">
            <Clock className="w-3 h-3" />
            <span>{timeAgo(plant.updated_at)}</span>
          </div>
        </div>
      </div>

      <div className="flex justify-center my-6">
        <CircularProgress value={plant.moisture} color={statusColor} />
      </div>

      <div className="grid grid-cols-2 gap-4 mt-6 pt-6 border-t border-slate-100 dark:border-slate-700">
        <div className="flex items-center gap-3">
          <div className="p-2 bg-blue-50 dark:bg-blue-900/20 rounded-lg text-blue-500">
            <Droplet className="w-5 h-5" />
          </div>
          <div>
            <p className="text-xs text-slate-500 dark:text-slate-400 font-medium">Sensor Value</p>
            <p className="text-sm font-bold text-slate-800 dark:text-white">{plant.moisture.toFixed(1)}%</p>
          </div>
        </div>
        
        <div className="flex items-center gap-3">
          <div className="p-2 bg-orange-50 dark:bg-orange-900/20 rounded-lg text-orange-500">
            <Thermometer className="w-5 h-5" />
          </div>
          <div>
            <p className="text-xs text-slate-500 dark:text-slate-400 font-medium">Temp (est)</p>
            <p className="text-sm font-bold text-slate-800 dark:text-white">
              {plant.temperature ? `${plant.temperature}°C` : '--'}
            </p>
          </div>
        </div>
      </div>
    </div>
  );
};

// --- Main Application ---

export default function App() {
  const [isSimulated, setIsSimulated] = useState(true);
  const [showSettings, setShowSettings] = useState(false);
  const [plants, setPlants] = useState({});
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);
  const [lastFetch, setLastFetch] = useState(null);

  // Supabase Configuration State
  const [config, setConfig] = useState({
    url: '',
    anonKey: '',
    tableName: 'sensor_data',
    pollingInterval: 5000 // 5 seconds
  });

  // Handle configuration changes
  const handleConfigChange = (e) => {
    setConfig({ ...config, [e.target.name]: e.target.value });
  };

  const connectSupabase = () => {
    if (!config.url || !config.anonKey || !config.tableName) {
      setError("Please provide URL, Anon Key, and Table Name.");
      return;
    }
    setIsSimulated(false);
    setShowSettings(false);
    setError(null);
    setPlants({});
    fetchSupabaseData();
  };

  const enableSimulation = () => {
    setIsSimulated(true);
    setShowSettings(false);
    setError(null);
  };

  // Fetch data from Supabase REST API
  const fetchSupabaseData = useCallback(async () => {
    if (isSimulated || !config.url) return;

    try {
      setLoading(true);
      // Fetch the last 100 records to ensure we get the latest for all plants
      const endpoint = `${config.url}/rest/v1/${config.tableName}?select=*&order=created_at.desc&limit=100`;
      
      const response = await fetch(endpoint, {
        headers: {
          'apikey': config.anonKey,
          'Authorization': `Bearer ${config.anonKey}`,
          'Content-Type': 'application/json',
          'Prefer': 'return=representation'
        }
      });

      if (!response.ok) {
        throw new Error(`Supabase Error: ${response.status} ${response.statusText}`);
      }

      const data = await response.json();
      
      // Process data: Group by plant ID/name and keep only the latest reading
      const latestReadings = {};
      data.forEach(row => {
        // Assume columns might be named differently, try to map common names
        const id = row.plant_id || row.device_id || row.id;
        const name = row.plant_name || row.name || `Sensor ${id}`;
        const moisture = row.moisture || row.humidity || row.value || 0;
        const temperature = row.temperature || row.temp || null;
        const updated_at = row.created_at || row.updated_at || new Date().toISOString();

        if (id && !latestReadings[id]) {
          latestReadings[id] = { id, name, moisture, temperature, updated_at };
        }
      });

      setPlants(latestReadings);
      setLastFetch(new Date());
      setError(null);
    } catch (err) {
      console.error("Fetch error:", err);
      setError(err.message);
    } finally {
      setLoading(false);
    }
  }, [config, isSimulated]);

  // Simulated Data Generator
  const generateSimulatedData = useCallback(() => {
    setPlants(prev => {
      const now = new Date().toISOString();
      // Initialize if empty
      if (Object.keys(prev).length === 0) {
        return {
          '1': { id: '1', name: 'Monstera Deliciosa', moisture: 65, temperature: 22.5, updated_at: now },
          '2': { id: '2', name: 'Fiddle Leaf Fig', moisture: 28, temperature: 24.1, updated_at: now },
          '3': { id: '3', name: 'Snake Plant', moisture: 85, temperature: 21.0, updated_at: now },
          '4': { id: '4', name: 'Pothos', moisture: 45, temperature: 23.2, updated_at: now },
        };
      }
      // Update slightly
      const next = { ...prev };
      Object.keys(next).forEach(key => {
        const change = (Math.random() - 0.5) * 5; // Change by -2.5% to +2.5%
        let newMoisture = next[key].moisture + change;
        newMoisture = Math.max(0, Math.min(100, newMoisture)); // Clamp 0-100
        
        next[key] = {
          ...next[key],
          moisture: newMoisture,
          updated_at: now
        };
      });
      return next;
    });
    setLastFetch(new Date());
    setLoading(false);
  }, []);

  // Polling Effect
  useEffect(() => {
    // Initial fetch
    if (isSimulated) {
      generateSimulatedData();
    } else {
      fetchSupabaseData();
    }

    // Set up polling interval
    const intervalId = setInterval(() => {
      if (isSimulated) {
        generateSimulatedData();
      } else {
        fetchSupabaseData();
      }
    }, isSimulated ? 3000 : config.pollingInterval); // Simulate updates faster

    return () => clearInterval(intervalId);
  }, [isSimulated, fetchSupabaseData, generateSimulatedData, config.pollingInterval]);


  const plantList = Object.values(plants);

  return (
    <div className="min-h-screen bg-slate-50 dark:bg-slate-900 text-slate-900 dark:text-slate-100 font-sans p-4 md:p-8">
      
      {/* Header */}
      <header className="max-w-6xl mx-auto flex flex-col md:flex-row items-start md:items-center justify-between mb-10 gap-4">
        <div>
          <h1 className="text-3xl font-extrabold tracking-tight flex items-center gap-3">
            <Droplet className="w-8 h-8 text-blue-500 fill-blue-500/20" />
            Hydration Hub
          </h1>
          <p className="text-slate-500 dark:text-slate-400 mt-1">Real-time plant sensor monitoring</p>
        </div>

        <div className="flex items-center gap-3">
          <div className={`flex items-center gap-2 px-4 py-2 rounded-full text-sm font-medium ${
            isSimulated 
              ? 'bg-orange-100 text-orange-700 dark:bg-orange-900/30 dark:text-orange-400' 
              : error 
                ? 'bg-red-100 text-red-700 dark:bg-red-900/30 dark:text-red-400'
                : 'bg-emerald-100 text-emerald-700 dark:bg-emerald-900/30 dark:text-emerald-400'
          }`}>
            {isSimulated ? <WifiOff className="w-4 h-4" /> : <Wifi className="w-4 h-4" />}
            {isSimulated ? 'Simulation Mode' : error ? 'Connection Error' : 'Live: Supabase'}
          </div>
          
          <button 
            onClick={() => setShowSettings(!showSettings)}
            className="p-2.5 bg-white dark:bg-slate-800 border border-slate-200 dark:border-slate-700 rounded-full hover:bg-slate-100 dark:hover:bg-slate-700 transition-colors shadow-sm"
            title="Settings"
          >
            <Settings className="w-5 h-5 text-slate-600 dark:text-slate-300" />
          </button>
        </div>
      </header>

      {/* Settings Modal / Panel */}
      {showSettings && (
        <div className="max-w-6xl mx-auto mb-8 bg-white dark:bg-slate-800 p-6 rounded-3xl shadow-lg border border-slate-200 dark:border-slate-700 animate-in fade-in slide-in-from-top-4">
          <div className="flex items-center gap-3 mb-6">
            <Database className="w-6 h-6 text-indigo-500" />
            <h2 className="text-xl font-bold">Connect to Supabase</h2>
          </div>
          
          <div className="grid grid-cols-1 md:grid-cols-2 gap-6 mb-6">
            <div>
              <label className="block text-sm font-medium text-slate-700 dark:text-slate-300 mb-2">Supabase Project URL</label>
              <input 
                type="text" 
                name="url"
                value={config.url}
                onChange={handleConfigChange}
                placeholder="https://your-project.supabase.co"
                className="w-full px-4 py-3 rounded-xl bg-slate-50 dark:bg-slate-900 border border-slate-200 dark:border-slate-700 focus:ring-2 focus:ring-indigo-500 outline-none transition-all"
              />
            </div>
            <div>
              <label className="block text-sm font-medium text-slate-700 dark:text-slate-300 mb-2">Anon / Public Key</label>
              <input 
                type="password" 
                name="anonKey"
                value={config.anonKey}
                onChange={handleConfigChange}
                placeholder="eyJh..."
                className="w-full px-4 py-3 rounded-xl bg-slate-50 dark:bg-slate-900 border border-slate-200 dark:border-slate-700 focus:ring-2 focus:ring-indigo-500 outline-none transition-all"
              />
            </div>
            <div>
              <label className="block text-sm font-medium text-slate-700 dark:text-slate-300 mb-2">Table Name</label>
              <input 
                type="text" 
                name="tableName"
                value={config.tableName}
                onChange={handleConfigChange}
                placeholder="sensor_data"
                className="w-full px-4 py-3 rounded-xl bg-slate-50 dark:bg-slate-900 border border-slate-200 dark:border-slate-700 focus:ring-2 focus:ring-indigo-500 outline-none transition-all"
              />
              <p className="text-xs text-slate-500 mt-2">
                Expected columns: <code>id</code>, <code>moisture</code>, <code>created_at</code>
              </p>
            </div>
            <div>
              <label className="block text-sm font-medium text-slate-700 dark:text-slate-300 mb-2">Update Frequency (ms)</label>
              <input 
                type="number" 
                name="pollingInterval"
                value={config.pollingInterval}
                onChange={handleConfigChange}
                step="1000"
                min="1000"
                className="w-full px-4 py-3 rounded-xl bg-slate-50 dark:bg-slate-900 border border-slate-200 dark:border-slate-700 focus:ring-2 focus:ring-indigo-500 outline-none transition-all"
              />
            </div>
          </div>

          <div className="flex gap-4">
            <button 
              onClick={connectSupabase}
              className="px-6 py-3 bg-indigo-600 hover:bg-indigo-700 text-white font-medium rounded-xl transition-colors shadow-sm"
            >
              Connect & Fetch
            </button>
            <button 
              onClick={enableSimulation}
              className="px-6 py-3 bg-slate-100 dark:bg-slate-700 hover:bg-slate-200 dark:hover:bg-slate-600 text-slate-800 dark:text-white font-medium rounded-xl transition-colors"
            >
              Run Demo Simulation
            </button>
          </div>
        </div>
      )}

      {/* Main Content Area */}
      <main className="max-w-6xl mx-auto">
        
        {/* Error State */}
        {error && (
          <div className="mb-8 p-4 bg-red-50 dark:bg-red-900/20 border-l-4 border-red-500 text-red-700 dark:text-red-400 rounded-r-xl flex items-start gap-3">
            <AlertCircle className="w-5 h-5 mt-0.5 flex-shrink-0" />
            <div>
              <h3 className="font-bold">Connection Failed</h3>
              <p className="text-sm mt-1">{error}</p>
              <button 
                onClick={() => setShowSettings(true)}
                className="mt-3 text-sm font-semibold underline hover:text-red-800 dark:hover:text-red-300"
              >
                Check Settings
              </button>
            </div>
          </div>
        )}

        {/* Loading State (Initial) */}
        {loading && plantList.length === 0 && !error && (
          <div className="flex flex-col items-center justify-center py-20 text-slate-500">
            <RefreshCw className="w-10 h-10 animate-spin mb-4 text-indigo-500" />
            <p className="text-lg font-medium">Fetching sensor data...</p>
          </div>
        )}

        {/* Empty State */}
        {!loading && plantList.length === 0 && !error && (
          <div className="text-center py-20 bg-white dark:bg-slate-800 rounded-3xl border border-slate-200 dark:border-slate-700 shadow-sm">
            <Leaf className="w-16 h-16 mx-auto text-slate-300 dark:text-slate-600 mb-4" />
            <h3 className="text-xl font-bold mb-2">No sensor data found</h3>
            <p className="text-slate-500 max-w-md mx-auto">
              Your Supabase table might be empty, or the columns don't match the expected format (id, moisture).
            </p>
          </div>
        )}

        {/* Dashboard Grid */}
        {plantList.length > 0 && (
          <>
            <div className="flex justify-between items-center mb-6 px-2">
              <h2 className="text-lg font-bold text-slate-700 dark:text-slate-200">
                Your Plants ({plantList.length})
              </h2>
              {lastFetch && (
                <p className="text-xs text-slate-500 flex items-center gap-1">
                  <RefreshCw className={`w-3 h-3 ${loading ? 'animate-spin' : ''}`} />
                  Last synced: {lastFetch.toLocaleTimeString()}
                </p>
              )}
            </div>
            
            <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 xl:grid-cols-4 gap-6">
              {plantList.map(plant => (
                <PlantCard key={plant.id} plant={plant} />
              ))}
            </div>
          </>
        )}
      </main>
      
    </div>
  );
}
