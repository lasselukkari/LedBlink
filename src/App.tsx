import { useEffect, useState } from 'react'
import './App.css'

async function requestLed(options?: RequestInit): Promise<boolean> {
  const response = await fetch('/led', { cache: 'no-store', ...options })
  if (!response.ok) {
    throw new Error(`Request failed: HTTP ${response.status}`)
  }

  const state = await response.text()
  if (state !== '0' && state !== '1') {
    throw new Error('The board returned an invalid LED state')
  }
  return state === '1'
}

export default function App() {
  const [ledOn, setLedOn] = useState<boolean | null>(null)
  const [busy, setBusy] = useState(false)
  const [error, setError] = useState('')

  useEffect(() => {
    const controller = new AbortController()
    requestLed({ signal: controller.signal })
      .then((state) => {
        if (!controller.signal.aborted) setLedOn(state)
      })
      .catch((cause: unknown) => {
        if (!controller.signal.aborted) {
          setError(cause instanceof Error ? cause.message : 'Cannot reach the board')
        }
      })

    return () => controller.abort()
  }, [])

  async function toggleLed() {
    if (ledOn === null || busy) return
    setBusy(true)
    setError('')

    try {
      const state = await requestLed({
        method: 'PUT',
        headers: { 'Content-Type': 'text/plain' },
        body: ledOn ? '0' : '1',
      })
      setLedOn(state)
    } catch (cause: unknown) {
      setError(cause instanceof Error ? cause.message : 'Cannot reach the board')
    } finally {
      setBusy(false)
    }
  }

  return (
    <main>
      <h1>ESP32 LED control</h1>
      <p>
        {ledOn === null
          ? error ? 'LED state unavailable' : 'Reading LED state…'
          : `The LED is ${ledOn ? 'on' : 'off'}.`}
      </p>
      <button
        type="button"
        disabled={ledOn === null || busy}
        onClick={toggleLed}
      >
        {busy ? 'Updating…' : ledOn ? 'Turn off' : 'Turn on'}
      </button>
      {error && <p role="alert">{error}. Check the board connection and reload to retry.</p>}
    </main>
  )
}
